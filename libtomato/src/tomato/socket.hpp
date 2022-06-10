#pragma once

#include <array>
#include <chrono>

#include <fmt/format.h>

#if defined(__linux__)
#include <sys/poll.h>
#endif

#include "address_family.hpp"
#include "socket_address.hpp"
#include "socket_error.hpp"
#include "socket_type.hpp"

namespace tomato
{

#if defined(_WIN32)
using NativeSocket = SOCKET;
#elif defined(__linux__)
using NativeSocket = int;
#endif

class Socket
{
public:
	Socket();
	explicit Socket(NativeSocket other);
	Socket(AddressFamily af, SocketType type);
	Socket(const Socket &) = delete;
	Socket(Socket &&other) noexcept;
	~Socket();

	Socket &operator=(const Socket &) = delete;
	Socket &operator=(Socket &&other) noexcept;

	[[nodiscard]] bool operator==(const Socket &other) const;
	[[nodiscard]] bool operator==(NativeSocket other) const;

	[[nodiscard]] explicit operator NativeSocket() const;

	void bind(const SocketAddress &address);
	void listen();
	void close();

	[[nodiscard]] Socket accept(std::chrono::milliseconds timeout = {}) const;
	[[nodiscard]] Socket accept(SocketAddress &connAddress, std::chrono::milliseconds timeout = {}) const;

	template<size_t size>
	size_t receive(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout = {}) const;

private:
	[[nodiscard]] short poll(short events, int timeout) const;

	NativeSocket handler;
};

template<size_t size>
size_t Socket::receive(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout) const
{
	using namespace std::chrono_literals;

	auto events = poll(POLLRDNORM, timeout == 0ms ? -1 : static_cast<int>(timeout.count()));

	if ((events & POLLRDNORM) != POLLRDNORM)
		return 0;

	auto received = ::recv(handler, reinterpret_cast<char *>(buffer.data()), size, 0);

	if (received < 0)
		throw SocketError(fmt::format("Could not read data. {}", socketErrorString()));

	return static_cast<size_t>(received);
}

} // namespace tomato
