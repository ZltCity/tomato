#pragma once

#include <array>
#include <chrono>

#include <fmt/format.h>

#include "socket_address.hpp"
#include "socket_error.hpp"
#include "socket_event.hpp"
#include "socket_shutdown.hpp"
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
	void shutdown(SocketShutdown how);
	void close();

	[[nodiscard]] SocketEvent wait(SocketEvent events, std::chrono::milliseconds timeout = {}) const;

	[[nodiscard]] Socket accept(std::chrono::milliseconds timeout = {}) const;
	[[nodiscard]] Socket accept(SocketAddress &connAddress, std::chrono::milliseconds timeout = {}) const;

	template<size_t size>
	size_t read(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout = {}) const;

private:
	NativeSocket handle;
};

template<size_t size>
size_t Socket::read(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout) const
{
	const auto events = wait(SocketEvent::ReadyRead, timeout);

	if ((events & SocketEvent::ReadyRead) != SocketEvent::None)
	{
		const auto received = recv(handle, reinterpret_cast<char *>(buffer.data()), size, 0);

		if (received < 0)
			throw SocketError(fmt::format("Could not read data. {}", socketErrorString()));

		return received;
	}

	return 0;
}

} // namespace tomato
