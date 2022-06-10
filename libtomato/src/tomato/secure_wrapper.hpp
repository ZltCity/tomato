#pragma once

#include <filesystem>
#include <memory>

#include <fmt/format.h>

#include "socket.hpp"
#include "ssl_context.hpp"
#include "ssl_error.hpp"

namespace tomato
{

class SecureWrapper
{
public:
	SecureWrapper(Socket socket, const SSLContext &sslContext);
	SecureWrapper(const SecureWrapper &) = delete;
	SecureWrapper(SecureWrapper &&other) noexcept;

	SecureWrapper &operator=(const SecureWrapper &) = delete;
	SecureWrapper &operator=(SecureWrapper &&other) noexcept;

	template<size_t size>
	size_t receive(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout = {}) const;

private:
	void accept();
	void release();

	Socket socket;
	std::unique_ptr<::SSL, void (*)(::SSL *)> ssl;
};

template<size_t size>
size_t SecureWrapper::receive(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout) const
{
	const auto events = socket.wait(SocketEvent::ReadyRead, timeout);

	if ((events & SocketEvent::ReadyRead) != SocketEvent::None)
	{
		auto received = SSL_read(ssl.get(), buffer.data(), buffer.size());

		if (received < 0)
			throw SocketError(fmt::format("Could not read data. {}", sslErrorString()));

		return static_cast<size_t>(received);
	}

	return 0;
}

} // namespace tomato
