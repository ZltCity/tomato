#if defined(__linux__)
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "tomato/socket.hpp"

namespace tomato
{

#if defined(_WIN32)

class WinSock
{
public:
	WinSock()
	{
		auto libData = WSAData {};

		if (WSAStartup(MAKEWORD(2, 2), &libData) != 0)
			throw SocketError("Could not initialize Winsock library.");
	}
	~WinSock()
	{
		WSACleanup();
	}
};

void closeSocket(SOCKET s)
{
	closesocket(s);
}

const auto invalidSocket = INVALID_SOCKET;

#elif defined(__linux__)

void closeSocket(int s)
{
	close(s);
}

const auto invalidSocket = -1;

#endif

Socket::Socket() : handle(invalidSocket)
{}

Socket::Socket(NativeSocket other) : handle(other)
{}

Socket::Socket(AddressFamily af, SocketType type) : handle(invalidSocket)
{
#if defined(_WIN32)
	static auto winSock = WinSock {};
#endif
	handle = socket(static_cast<int>(af), static_cast<int>(type), 0);

	if (handle == invalidSocket)
		throw SocketError(fmt::format("Could not create socket. {}", socketErrorString()));

#if defined(_WIN32)
#define IOCTL ioctlsocket
	auto mode = u_long {1u};
#elif defined(__linux__)
#define IOCTL ioctl
	auto mode = int {1};
#endif

	if (IOCTL(handle, FIONBIO, &mode) != 0)
	{
		closeSocket(handle);

		throw SocketError(fmt::format("Could not set IO control option. {}", socketErrorString()));
	}

#undef IOCTL
}

Socket::Socket(Socket &&other) noexcept : handle(invalidSocket)
{
	std::swap(handle, other.handle);
}

Socket::~Socket()
{
	close();
}

Socket &Socket::operator=(Socket &&other) noexcept
{
	close();
	std::swap(handle, other.handle);

	return *this;
}

bool Socket::operator==(const Socket &other) const
{
	return handle == other.handle;
}

bool Socket::operator==(NativeSocket other) const
{
	return handle == other;
}

Socket::operator NativeSocket() const
{
	return handle;
}

void Socket::bind(const SocketAddress &address)
{
	const auto sas = static_cast<sockaddr_storage>(address);

	if (::bind(handle, reinterpret_cast<const sockaddr *>(&sas), sizeof(sas)) != 0)
		throw SocketError(fmt::format("Could not bind socket. {}", socketErrorString()));
}

void Socket::listen()
{
	if (::listen(handle, SOMAXCONN) != 0)
		throw SocketError(fmt::format("Could not set listen state. {}", socketErrorString()));
}

void Socket::shutdown(SocketShutdown how)
{
	if (::shutdown(handle, static_cast<int>(how)) != 0)
		throw SocketError(fmt::format("Could not shutdown the socket. {}", socketErrorString()));
}

void Socket::close()
{
	if (handle != invalidSocket)
	{
		closeSocket(handle);

		handle = invalidSocket;
	}
}

SocketEvent Socket::wait(SocketEvent events, std::chrono::milliseconds timeout) const
{
	using namespace std::chrono_literals;

	auto fd = pollfd {handle, static_cast<short>(events)};

#if defined(_WIN32)
#define POLL WSAPoll
#elif defined(__linux__)
#define POLL poll
#endif

	if (POLL(&fd, 1, timeout == 0ms ? -1 : static_cast<int>(timeout.count())) < 0)
		throw SocketError(fmt::format("Could not poll socket events. {}", socketErrorString()));

	return static_cast<SocketEvent>(fd.revents);
}

Socket Socket::accept(std::chrono::milliseconds timeout) const
{
	auto connAddress = SocketAddress {};

	return accept(connAddress, timeout);
}

Socket Socket::accept(SocketAddress &connAddress, std::chrono::milliseconds timeout) const
{
	const auto events = wait(SocketEvent::ReadyRead, timeout);

	if ((events & SocketEvent::ReadyRead) != SocketEvent::None)
	{
		auto sas = sockaddr_storage {};

#if defined(_WIN32)
		auto addrLen = static_cast<int>(sizeof(sas));
#elif defined(__linux__)
		auto addrLen = static_cast<socklen_t>(sizeof(sas));
#endif

		auto conn = ::accept(handle, reinterpret_cast<sockaddr *>(&sas), &addrLen);

		if (conn == invalidSocket)
			throw SocketError(fmt::format("Could not accept inbound connection. {}", socketErrorString()));

		connAddress = SocketAddress {sas};

		return Socket {conn};
	}

	return Socket {};
}

} // namespace tomato
