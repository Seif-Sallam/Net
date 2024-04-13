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

	Socket
	socket_create()
	{
		return {};
	}

	Socket_Error
	socket_close(Socket& self)
	{
		return {};
	}

	Socket_Error
	socket_connect(Socket& self, const IP_Endpoint& endpoint, size_t timeout)
	{
		return {};
	}

	Socket_Error
	socket_bind(Socket& self, const IP_Endpoint& endpoint)
	{
		return {};
	}

	Socket_Error
	socket_listen(Socket& self, const IP_Endpoint& endpoint)
	{
		return {};
	}

	Result<size_t>
	socket_send(Socket& self, Block& block)
	{
		return {};
	}

	Result<size_t>
	socket_receive(Socket& self, Block& block)
	{
		return {};
	}

	Result<Socket>
	socket_accept(Socket& self, size_t timeout)
	{
		return {};
	}

	Socket_Error
	socket_set_blocking(Socket& self, bool blocking)
	{
		return {};
	}
}