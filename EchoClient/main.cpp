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

	net::IP_Endpoint endpoint = net::ip_endpoint_create("127.0.0.1", 3000);
	net::ip_endpoint_print(endpoint);

	if (net::socket_connect(socket, endpoint, 0) != net::Socket_Error::NONE)
	{
		std::cout << "Failed to connect to server" << std::endl;
		return 1;
	}

	auto [bytes, err] = net::socket_send(socket, net::Block("Hello, World - CLIENT!"));
	if (err != net::Socket_Error::NONE)
	{
		std::cout << "Failed to send data" << std::endl;
		return 1;
	}

	std::string buf{};
	buf.resize(256);
	auto [bytes2, err2] = net::socket_receive(socket, net::Block{buf.data(), buf.size()});
	if (err2 != net::Socket_Error::NONE)
	{
		std::cout << "Failed to receive data" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Received: " << buf << std::endl;
	}

	net::socket_close(socket);

	return 0;
}