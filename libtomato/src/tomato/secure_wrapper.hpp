#pragma once

#include <filesystem>
#include <memory>

#include <fmt/format.h>
#include <openssl/ssl.h>

#include "socket.hpp"
#include "ssl_error.hpp"

namespace tomato
{

class SecureWrapper
{
public:
	SecureWrapper(Socket socket, const std::filesystem::path &cert, const std::filesystem::path &key);
	SecureWrapper(const SecureWrapper &) = delete;
	SecureWrapper(SecureWrapper &&other) noexcept;

	SecureWrapper &operator=(const SecureWrapper &) = delete;
	SecureWrapper &operator=(SecureWrapper &&other) noexcept;

	template<size_t size>
	size_t receive(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout = {}) const;

private:
	void close();

	Socket socket;
	std::unique_ptr<SSL_CTX, void (*)(SSL_CTX *)> context;
	std::unique_ptr<SSL, void (*)(SSL *)> ssl;
};

template<size_t size>
size_t SecureWrapper::receive(std::array<std::byte, size> &buffer, std::chrono::milliseconds timeout) const
{
}

} // namespace tomato
