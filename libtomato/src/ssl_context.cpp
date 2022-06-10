#include <fmt/format.h>

#include "tomato/ssl_context.hpp"
#include "tomato/ssl_error.hpp"

namespace tomato
{

SSLContext::SSLContext(const std::filesystem::path &cert, const std::filesystem::path &key)
	: handle(SSL_CTX_new(TLS_server_method()), SSL_CTX_free)
{
	if (handle == nullptr)
		throw SSLError(fmt::format("Could not create SSL context. {}", sslErrorString()));

	auto nativeHandle = handle.get();

	if (SSL_CTX_use_certificate_file(nativeHandle, cert.string().c_str(), SSL_FILETYPE_PEM) != 1)
		throw SSLError(fmt::format("Invalid certificate. {}", sslErrorString()));

	if (SSL_CTX_use_PrivateKey_file(nativeHandle, key.string().c_str(), SSL_FILETYPE_PEM) != 1)
		throw SSLError(fmt::format("Invalid private key. {}", sslErrorString()));
}

SSLContext::SSLContext(SSLContext &&other) noexcept : handle(nullptr, SSL_CTX_free)
{
	std::swap(handle, other.handle);
}

SSLContext &SSLContext::operator=(SSLContext &&other) noexcept
{
	release();
	std::swap(handle, other.handle);

	return *this;
}

SSLContext::operator SSL_CTX *() const
{
	return handle.get();
}

void SSLContext::release()
{
	handle = {};
}

} // namespace tomato
