#pragma once

#include <filesystem>
#include <memory>

#include <fmt/format.h>

#include "socket.hpp"
#include "ssl_context.hpp"
#include "ssl_error.hpp"

namespace tomato
{

class SecureWrapper final : public Pipe
{
public:
	SecureWrapper(Socket socket, const SSLContext &sslContext);
	SecureWrapper(const SecureWrapper &) = delete;
	SecureWrapper(SecureWrapper &&other) noexcept;

	SecureWrapper &operator=(const SecureWrapper &) = delete;
	SecureWrapper &operator=(SecureWrapper &&other) noexcept;

	void shutdown();

	size_t read(std::byte *buffer, size_t length, std::chrono::milliseconds timeout, bool peek) final;
	size_t write(const std::byte *buffer, size_t length, std::chrono::milliseconds timeout) final;

	static const uint32_t defaultTimeout = 30; // ms

private:
	void accept();
	void release();
	bool waitSocket(int retCode) const;

	Socket socket;
	std::unique_ptr<::SSL, void (*)(SSL *)> ssl;
};

} // namespace tomato
