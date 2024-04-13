#include <net/socket.h>

#include <iostream>

int main(int argc, const char* argv[])
{
	std::cout << "Hello, EchoServer!" << std::endl;

	net::Socket socket = net::socket_create();
	if (!socket)
	{
		std::cout << "Failed to create socket" << std::endl;
		return 1;
	}

	net::IP_Endpoint endpoint = net::ip_endpoint_create("127.0.0.1", 3000);
	net::ip_endpoint_print(endpoint);

	if (net::socket_listen(socket, endpoint) != net::Socket_Error::NONE)
	{
		std::cout << "Failed to listen on endpoint" << std::endl;
		return 1;
	}

	while(true)
	{
		auto [client, err] = net::socket_accept(socket, 0);

		if (err != net::Socket_Error::NONE)
		{
			std::cout << "Failed to accept client" << std::endl;
			return 1;
		}

		std::string buf{};
		buf.resize(256);
		auto [bytes, err1] = net::socket_receive(client, net::Block{buf.data(), buf.size()});
		if (err1 != net::Socket_Error::NONE)
		{
			std::cout << "Failed to receive data" << std::endl;
			return 1;
		}
		else
		{
			std::cout << "Received: " << buf << std::endl;
		}

		auto [sizes, err2] = net::socket_send(client, net::Block("Hello, World! - SERVER!"));
		if (err2 != net::Socket_Error::NONE)
		{
			std::cout << "Failed to send data" << std::endl;
			return 1;
		}

		net::socket_close(client);
	}
	return 0;
}