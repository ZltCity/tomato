#pragma once

#include <filesystem>
#include <memory>

#include <openssl/ssl.h>

namespace tomato
{

class SSLContext
{
public:
	SSLContext(const std::filesystem::path &cert, const std::filesystem::path &key);
	SSLContext(const SSLContext &) = delete;
	SSLContext(SSLContext &&other) noexcept;

	SSLContext &operator=(const SSLContext &) = delete;
	SSLContext &operator=(SSLContext &&other) noexcept;

	[[nodiscard]] explicit operator ::SSL_CTX *() const;

private:
	void release();

	std::unique_ptr<::SSL_CTX, void (*)(::SSL_CTX *)> handle;
};

} // namespace tomato
