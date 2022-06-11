#pragma once

#if defined(_WIN32)
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
#endif

namespace tomato
{

enum class SocketShutdown
{
#if defined(_WIN32)
	Read = SD_RECEIVE,
	Write = SD_SEND,
	Both = SD_BOTH
#elif defined(__linux__)
	Read = SHUT_RD,
	Write = SHUT_WR,
	Both = SHUT_RDWR
#endif
};

} // namespace tomato
