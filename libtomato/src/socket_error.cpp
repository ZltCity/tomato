#if defined(_WIN32)
#include <winsock2.h>
#elif defined(__linux__)
#include <cerrno>
#include <cstring>
#endif

#include "tomato/socket_error.hpp"

namespace tomato
{

SocketError::SocketError(const char *_what) : std::runtime_error(_what)
{}

SocketError::SocketError(const std::string &_what) : std::runtime_error(_what)
{}

#if defined(_WIN32)

int socketError()
{
	return WSAGetLastError();
}

std::string socketErrorString()
{
	return socketErrorString(WSAGetLastError());
}

std::string socketErrorString(int err)
{
	auto message = std::string {};
	char *buffer = nullptr;

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, 0,
		reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

	if (buffer != nullptr)
	{
		message = std::string(buffer);

		LocalFree(buffer);
	}

	return message;
}

#elif defined(__linux__)

int socketError()
{
	return errno;
}

std::string socketErrorString()
{
	return socketErrorString(errno);
}

std::string socketErrorString(int err)
{
	return {std::strerror(err)};
}

#endif

} // namespace tomato
