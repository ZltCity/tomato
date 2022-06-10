#pragma once

#if defined(_WIN32)
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
#endif

namespace tomato
{

enum class SocketType
{
	Stream = SOCK_STREAM,
	Datagram = SOCK_DGRAM
};

} // namespace tomato
