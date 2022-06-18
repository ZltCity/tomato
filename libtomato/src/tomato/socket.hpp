#pragma once

#include <array>
#include <chrono>

#include <fmt/format.h>

#include "pipe.hpp"
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

class Socket final : public Pipe
{
public:
	Socket();
	explicit Socket(NativeSocket other);
	Socket(AddressFamily af, SocketType type);
	Socket(const Socket &) = delete;
	Socket(Socket &&other) noexcept;
	~Socket() final;

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

	size_t read(std::byte *buffer, size_t length, std::chrono::milliseconds timeout, bool peek) final;
	size_t write(const std::byte *buffer, size_t length, std::chrono::milliseconds timeout) final;

private:
	NativeSocket handle;
};

} // namespace tomato
