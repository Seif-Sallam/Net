#include <net/socket.h>

#include <iostream>

int main(int argc, const char* argv[])
{
	std::cout << "Hello, EchoClient!" << std::endl;

	net::Socket socket = net::socket_create();
	if (!socket)
	{
		std::cout << "Failed to create socket" << std::endl;
		return 1;
	}

	net::IP_Endpoint endpoint = net::ip_endpoint_create("127.0.0.1", 12341);
	net::ip_endpoint_print(endpoint);

	while(true)
	{
		auto err = net::socket_connect(socket, endpoint, 2000);
		if (err == net::Socket_Error::TIMEOUT)
		{
			std::cout << "Connection Timeout" << std::endl;
			continue;
		}
		else if (err != net::Socket_Error::NONE)
		{
			std::cout << "Failed to connect to endpoint" << std::endl;
			return 1;
		}
		else
		{
			std::cout << "Connected to endpoint" << std::endl;
			break;
		}
	}

	while (true)
	{
		auto [bytes, err] = net::socket_send(socket, net::Block("Hello, World - CLIENT!"), 1000);
		if (err == net::Socket_Error::TIMEOUT)
		{
			std::cout << "Socket Write Timeout" << std::endl;
			continue;
		}
		else if (err != net::Socket_Error::NONE)
		{
			std::cout << "Failed to send data" << std::endl;
			return 1;
		}
		else
		{
			std::cout << "Sent: " << bytes << " bytes" << std::endl;
			break;
		}
	}

	std::string buf{};
	buf.resize(256);
	while(true)
	{
		auto [bytes2, err2] = net::socket_receive(socket, net::Block{buf.data(), buf.size()}, 1000);
		if (err2 == net::Socket_Error::TIMEOUT)
		{
			std::cout << "Socket Read Timeout" << std::endl;
			continue;
		}
		else if (err2 != net::Socket_Error::NONE)
		{
			std::cout << "Failed to send data" << std::endl;
			return 1;
		}
		else
		{
			std::cout << "Received: " << buf << std::endl;
			break;
		}
	}

	net::socket_close(socket);

	return 0;
}