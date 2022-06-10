#pragma once

#include <cstdint>

#if defined(_WIN32)
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/poll.h>
#endif

namespace tomato
{

enum class SocketEvent : uint32_t
{
	None = 0,
	ReadyRead = POLLRDNORM,
	ReadyWrite = POLLWRNORM,
	Any = ReadyRead | ReadyWrite
};

SocketEvent operator&(SocketEvent a, SocketEvent b);
SocketEvent operator|(SocketEvent a, SocketEvent b);

} // namespace tomato
