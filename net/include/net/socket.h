#pragma once
#include <string>

#include "net/Exports.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace net
{
	struct Socket_Error
	{
		enum Error
		{
			NONE = 0,
			GENERIC_ERROR,
		} error = NONE;

		operator bool() const {
			return error == NONE;
		}
	};

	template<typename T>
	struct Result : public Socket_Error
	{
		T value;

		Result() = default;
		Result(Error error)
			: Socket_Error{ error }
		{}
		Result(T value)
			: Socket_Error{ NONE }, value(value)
		{}

		operator T() const {
			return value;
		}
	};

	struct Block
	{
		Block(std::string data)
			: data(data.data()), size(data.size())
		{}
		char* data;
		size_t size;
	};

	struct IPEndPoint
	{
		std::string address;
		unsigned short port;
	};

	struct Socket
	{
		SOCKET handle;
		IPEndPoint endpoint;

		operator bool() const {
			return handle != INVALID_SOCKET;
		}
	};

	NET_EXPORT Socket
	socket_create();

	NET_EXPORT Socket_Error
	socket_close(Socket& socket);

	NET_EXPORT Socket_Error
	socket_connect(Socket& socket, const IPEndPoint& endpoint);

	NET_EXPORT Socket_Error
	socket_bind(Socket& socket, const IPEndPoint& endpoint);

	NET_EXPORT Socket_Error
	socket_listen(Socket& socket, const IPEndPoint& endpoint);

	NET_EXPORT Socket_Error
	socket_send(Socket& socket, Block& block);

	NET_EXPORT Socket_Error
	socket_send_all(Socket& socket, Block& block);

	NET_EXPORT Socket_Error
	socket_receive(Socket& socket, Block& block);

	NET_EXPORT Socket_Error
	socket_receive_all(Socket& socket, Block& block);

	NET_EXPORT Result<Socket>
	socket_accept(Socket& socket);

	NET_EXPORT Socket_Error
	socket_set_blocking(Socket& socket, bool blocking);
}