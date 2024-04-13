#pragma once
#include <string>
#include <array>

#include "net/Exports.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace net
{
	enum Socket_Error
	{
		NONE = 0,
		INVALID_HANDLE,
		GENERIC_ERROR,
	};

	template<typename T>
	struct Result
	{
		T value;
		Socket_Error error;

		Result() = default;
		Result(Socket_Error error)
			: error(error)
		{}
		Result(T value)
			: error(Socket_Error::NONE), value(value)
		{}

		operator T() const {
			return value;
		}
	};

	struct Block
	{
		Block(const char* data, size_t size)
			: data(const_cast<char*>(data)), size(size)
		{}
		Block(const char* data)
			: data(const_cast<char*>(data)), size(strlen(data))
		{}

		char* data;
		size_t size;
	};

	struct IP_Endpoint
	{
		std::string hostname;
		std::string ip_string;
		union {
			std::array<uint8_t, 4> ipv4_bytes;
			std::array<uint8_t, 16> ipv6_bytes;
		};
		unsigned short port;

		enum Version
		{
			IPV4,
			IPV6,
		} version = IPV4;
	};

	NET_EXPORT IP_Endpoint
	ip_endpoint_create(const char* ip_address, unsigned short port);

	NET_EXPORT IP_Endpoint
	ip_endpoint_create(sockaddr* addr);

	NET_EXPORT void
	ip_endpoint_print(const IP_Endpoint& endpoint);

	struct Socket
	{
		SOCKET handle;
		IP_Endpoint endpoint;

		operator bool() const {
			return handle != INVALID_SOCKET;
		}
	};

	NET_EXPORT Result<Socket>
	socket_create();

	NET_EXPORT Socket_Error
	socket_close(Socket& self);

	NET_EXPORT Socket_Error
	socket_connect(Socket& self, const IP_Endpoint& endpoint, size_t timeout);

	NET_EXPORT Socket_Error
	socket_bind(Socket& self, const IP_Endpoint& endpoint);

	NET_EXPORT Socket_Error
	socket_listen(Socket& self, const IP_Endpoint& endpoint, int backlog = 5);

	NET_EXPORT Result<size_t>
	socket_send(Socket& self, Block& block);

	NET_EXPORT Result<size_t>
	socket_receive(Socket& self, Block& block);

	NET_EXPORT Result<Socket>
	socket_accept(Socket& self, size_t timeout);

	NET_EXPORT Socket_Error
	socket_set_blocking(Socket& socket, bool blocking);
}