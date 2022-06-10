#pragma once

#if defined(_WIN32)
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
#endif

namespace tomato
{

enum class AddressFamily
{
	Inet = AF_INET,
	Inet6 = AF_INET6
};

} // namespace tomato
