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

	void shutdown();

	template<size_t size>
	size_t read(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout = {}) const;
	template<size_t size>
	size_t write(const std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout = {});

	static const uint32_t defaultTimeout = 30; // ms

private:
	void accept();
	void release();
	void waitSocket(int retCode) const;

	Socket socket;
	std::unique_ptr<::SSL, void (*)(SSL *)> ssl;
};

template<size_t size>
size_t SecureWrapper::read(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout) const
{
	sslClearErrorStack();

	auto received = int {};

	while ((received = SSL_read(ssl.get(), buffer.data(), size)) <= 0)
	{
		waitSocket(SSL_get_error(ssl.get(), received));
	}

	return received;
}

template<size_t size>
size_t SecureWrapper::write(const std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout)
{
	sslClearErrorStack();

	auto sent = int {};

	while ((sent = SSL_write(ssl.get(), buffer.data(), size)) <= 0)
	{
		waitSocket(SSL_get_error(ssl.get(), sent));
	}

	return sent;
}

} // namespace tomato
