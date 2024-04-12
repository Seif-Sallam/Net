#include "net/socket.h"

namespace net
{
	Socket
	socket_create()
	{
		return {};
	}

	Socket_Error
	socket_close(Socket& socket)
	{
		return {};
	}

	Socket_Error
	socket_connect(Socket& socket, const IPEndPoint& endpoint)
	{
		return {};
	}

	Socket_Error
	socket_bind(Socket& socket, const IPEndPoint& endpoint)
	{
		return {};
	}

	Socket_Error
	socket_listen(Socket& socket, const IPEndPoint& endpoint)
	{
		return {};
	}

	Socket_Error
	socket_send(Socket& socket, Block& block)
	{
		return {};
	}

	Socket_Error
	socket_send_all(Socket& socket, Block& block)
	{
		return {};
	}

	Socket_Error
	socket_receive(Socket& socket, Block& block)
	{
		return {};
	}

	Socket_Error
	socket_receive_all(Socket& socket, Block& block)
	{
		return {};
	}

	Result<Socket>
	socket_accept(Socket& socket)
	{
		return {};
	}

	Socket_Error
	socket_set_blocking(Socket& socket, bool blocking)
	{
		return {};
	}
}