#include "net/socket.h"
#include <iostream>

namespace net
{
	struct _Winsock_Context
	{
		_Winsock_Context()
		{ WSADATA wsa_data; WSAStartup(MAKEWORD(2, 2), &wsa_data); }
		~_Winsock_Context() { WSACleanup(); }
	};

	static _Winsock_Context winsock_context;

	inline static sockaddr_in
	_ip_endpoint_as_sockaddr(const IP_Endpoint& self)
	{
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(self.port);
		memcpy(&addr.sin_addr, self.ipv4_bytes.data(), sizeof(ULONG));
		return addr;
	}

	inline static sockaddr_in6
	_ip_endpoint_as_sockaddr6(const IP_Endpoint& self)
	{
		sockaddr_in6 addr = {};
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(self.port);
		memcpy(&addr.sin6_addr, self.ipv6_bytes.data(), sizeof(ULONG));
		return addr;
	}

	inline static bool
	_socket_would_block(int error_code)
	{
		return error_code == WSAEWOULDBLOCK || error_code == WSAEINPROGRESS;
	}

	inline static timeval
	_calculate_timeout_in_sec(uint32_t timeout_in_ms)
	{
		timeval timeout{};

		if (timeout_in_ms >= 1000)
		{
			timeout.tv_sec = timeout_in_ms / 1000;
			timeout_in_ms -= timeout.tv_sec * 1000;
		}

		timeout.tv_usec = timeout_in_ms * 1000;
		return timeout;
	}

	typedef enum
	{
		WAIT_NONE     = 0x0,
		WAIT_ON_WRITE = 0x1,
		WAIT_ON_READ  = 0x2,
	} Wait_Mode;

	inline static int
	_socket_wait_on(int64_t socket_fd, timeval* timeout, Wait_Mode mode)
	{
		fd_set read_set;
		fd_set write_set;

		FD_ZERO(&read_set);
		FD_ZERO(&write_set);

		if (mode & WAIT_ON_READ)
			FD_SET(socket_fd, &read_set);

		if (mode & WAIT_ON_WRITE)
			FD_SET(socket_fd, &write_set);

		int ready_sockets = ::select(socket_fd + 1, &read_set, &write_set, nullptr, timeout);

		return ready_sockets;
	}

	IP_Endpoint
	ip_endpoint_create(const char* ip_address, unsigned short port)
	{
		IP_Endpoint self{};
		self.port = port;

		in_addr addr; //location to store the ipv4 address
		int result = inet_pton(AF_INET, ip_address, &addr);

		if (result == 1)
		{
			if (addr.S_un.S_addr != INADDR_NONE)
			{
				self.ip_string = std::string(ip_address);
				self.hostname = std::string(ip_address);

				memcpy(self.ipv4_bytes.data(), &addr.S_un.S_addr, sizeof(ULONG));
				self.version = IP_Endpoint::IPV4;
				return self;
			}
		}

		//Attempt to resolve hostname to ipv4 address
		addrinfo hints = {}; //hints will filter the results we get back for getaddrinfo
		hints.ai_family = AF_INET; //ipv4 addresses only

		addrinfo * hostinfo = nullptr;
		result = getaddrinfo(ip_address, NULL, &hints, &hostinfo);
		if (result == 0)
		{
			sockaddr_in * host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

			self.ip_string.resize(16);
			inet_ntop(AF_INET, &host_addr->sin_addr, &self.ip_string[0], 16);

			self.hostname = std::string(ip_address);

			ULONG ip_long = host_addr->sin_addr.S_un.S_addr; //get ip address as unsigned long
			memcpy(self.ipv4_bytes.data(), &ip_long, sizeof(ULONG)); //copy bytes into our array of bytes representing ip address

			self.version = IP_Endpoint::IPV4;

			freeaddrinfo(hostinfo); //memory cleanup from getaddrinfo call
			return self;
		}

		return self;
	}

	IP_Endpoint
	ip_endpoint_create(sockaddr* addr)
	{
		IP_Endpoint self{};
		if (addr->sa_family == AF_INET)
		{
			sockaddr_in* ipv4_addr = reinterpret_cast<sockaddr_in*>(addr);
			self.version = IP_Endpoint::IPV4;
			self.port = ntohs(ipv4_addr->sin_port);
			self.ip_string.resize(16);
			inet_ntop(AF_INET, &ipv4_addr->sin_addr, &self.ip_string[0], 16);
			memcpy(self.ipv4_bytes.data(), &ipv4_addr->sin_addr.S_un.S_addr, sizeof(ULONG));
		}
		else if (addr->sa_family == AF_INET6)
		{
			sockaddr_in6* ipv6_addr = reinterpret_cast<sockaddr_in6*>(addr);
			self.version = IP_Endpoint::IPV6;
			self.port = ntohs(ipv6_addr->sin6_port);
			self.ip_string.resize(46);
			inet_ntop(AF_INET6, &ipv6_addr->sin6_addr, &self.ip_string[0], 46);
			memcpy(self.ipv6_bytes.data(), &ipv6_addr->sin6_addr.u.Byte, 16);
		}
		self.hostname = self.ip_string;
		return self;
	}

	void
	ip_endpoint_print(const IP_Endpoint& self)
	{
		std::cout << ((self.version == IP_Endpoint::IPV4) ? "IPV4\n" : "IPV6\n");
		std::cout << "Hostname: " << self.hostname << '\n';
		std::cout << "IP Address: " << self.ip_string << '\n';
		std::cout << "Port: " << self.port << '\n';
		std::cout << "Bytes: ";
		if (self.version == IP_Endpoint::IPV4)
			for (auto byte : self.ipv4_bytes)
				std::cout << '[' <<(int)byte << ']';
		else
			for (auto byte : self.ipv6_bytes)
				std::cout << '[' << (int)byte << ']';
		std::cout << '\n';
	}

	Result<Socket>
	socket_create()
	{
		Socket self{};
		self.handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (!self)
		{
			int error = WSAGetLastError();
			return Socket_Error::INVALID_HANDLE;
		}

		// Disable Nagle's algorithm
		int value = 1;
		int res = setsockopt(self.handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
		if (res != 0)
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}

		if (socket_set_blocking(self, false) != Socket_Error::NONE)
			return Socket_Error::GENERIC_ERROR;

		return self;
	}

	Socket_Error
	socket_close(Socket& self)
	{
		if (self)
		{
			int res = closesocket(self.handle);
			if (res != 0) // Error
				return Socket_Error::GENERIC_ERROR;
			self.handle = INVALID_SOCKET;
		}
		else
		{
			return Socket_Error::INVALID_HANDLE;
		}
		return {};
	}

	Socket_Error
	socket_connect(Socket& self, const IP_Endpoint& endpoint, uint32_t timeout)
	{
		if (!self)
			return Socket_Error::INVALID_HANDLE;
		self.endpoint = endpoint;

		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(endpoint.port);
		memcpy(&addr.sin_addr, endpoint.ipv4_bytes.data(), sizeof(ULONG));

		auto t = _calculate_timeout_in_sec(timeout);

		for(;;)
		{
			// Non blocking sockets do not connect immediately, we need to wait for the socket to become writable
			int result = connect(self.handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
			// Will always return error in the beginning
			if (result != -1)
				break;

			auto error_code = WSAGetLastError();
			if (_socket_would_block(error_code) == false)
			{
				if (error_code == WSAEISCONN)
					break;
				return Socket_Error::GENERIC_ERROR;
			}

			int ready = _socket_wait_on(self.handle, &t, WAIT_ON_WRITE);
			if (ready == 0)
				return Socket_Error::TIMEOUT;

			if (ready == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				return Socket_Error::GENERIC_ERROR;
			}

			// the socket is writable now, but we need to check that SO_ERROR is set to 0, this is an
			// indication that the connection was established successfully
			{
				int error_code = 0;
				socklen_t error_len = sizeof(error_code);

				int status = ::getsockopt(self.handle, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_len);
				if (status == -1)
					return Socket_Error::GENERIC_ERROR;

				if (error_code != 0)
					return Socket_Error::GENERIC_ERROR;
			}
		}

		return {};
	}

	Socket_Error
	socket_bind(Socket& self, const IP_Endpoint& endpoint)
	{
		if (!self)
			return Socket_Error::INVALID_HANDLE;
		self.endpoint = endpoint;

		sockaddr_in addr = _ip_endpoint_as_sockaddr(endpoint);
		int result = bind(self.handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0) //if an error occurred
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}
		return {};
	}

	Socket_Error
	socket_listen(Socket& self, const IP_Endpoint& endpoint, int backlog)
	{
		if (!self)
			return Socket_Error::INVALID_HANDLE;

		if (socket_bind(self, endpoint) != Socket_Error::NONE)
			return Socket_Error::GENERIC_ERROR;

		if (backlog == 0)
			backlog = SOMAXCONN;

		int result = listen(self.handle, backlog);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}

		return {};
	}

	Result<size_t>
	socket_send(Socket& self, Block& block, uint32_t timeout)
	{
		if (!self)
			return Socket_Error::INVALID_HANDLE;

		auto t = _calculate_timeout_in_sec(timeout);

		int ready = _socket_wait_on(self.handle, &t, Wait_Mode::WAIT_ON_WRITE);
		if (ready == -1)
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}

		if (ready == 0)
			return Socket_Error::TIMEOUT;

		int bytes_sent = send(self.handle, block.data, block.size, 0);
		if (bytes_sent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}

		return bytes_sent;
	}

	Result<size_t>
	socket_receive(Socket& self, Block& block, uint32_t timeout)
	{
		if (!self)
			return Socket_Error::INVALID_HANDLE;

		auto t = _calculate_timeout_in_sec(timeout);

		int ready = _socket_wait_on(self.handle, &t, Wait_Mode::WAIT_ON_READ);
		if (ready == -1)
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}

		if (ready == 0)
			return Socket_Error::TIMEOUT;

		int bytes_received = recv(self.handle, block.data, block.size, 0);
		if (bytes_received == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return Socket_Error::GENERIC_ERROR;
		}

		return bytes_received;
	}

	Result<Socket>
	socket_accept(Socket& self, uint32_t timeout)
	{
		if (!self)
			return Socket_Error::INVALID_HANDLE;

		auto t = _calculate_timeout_in_sec(timeout);

		sockaddr_in addr = {};
		int len = sizeof(sockaddr_in);
		for(;;)
		{
			SOCKET conn = accept(self.handle, (sockaddr*)(&addr), &len);
			if (conn != INVALID_SOCKET)
			{
				IP_Endpoint conn_endpoint = ip_endpoint_create((sockaddr*)&addr);
				Socket out_socket{};
				out_socket.handle = conn;
				out_socket.endpoint = conn_endpoint;
				return out_socket;
			}

			auto error_code = WSAGetLastError();
			if (_socket_would_block(error_code) == false)
				return Socket_Error::GENERIC_ERROR;

			int ready = _socket_wait_on(self.handle, &t, WAIT_ON_READ);
			if (ready == 0)
				return Socket_Error::TIMEOUT;

			if (ready == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				return Socket_Error::GENERIC_ERROR;
			}
		}

		return {};
	}

	Socket_Error
	socket_set_blocking(Socket& self, bool blocking)
	{
		u_long is_blocking = (blocking) ? 0UL : 1UL;
		int result = ioctlsocket(self.handle, FIONBIO, &is_blocking);
		if (result == SOCKET_ERROR)
			return Socket_Error::GENERIC_ERROR;

		return Socket_Error::NONE;
	}
}