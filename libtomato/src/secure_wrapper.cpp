#include <openssl/err.h>

#include "tomato/secure_wrapper.hpp"

namespace tomato
{

SecureWrapper::SecureWrapper(Socket socket, const std::filesystem::path &cert, const std::filesystem::path &key)
	: context(SSL_CTX_new(TLS_server_method()), SSL_CTX_free), ssl(nullptr, SSL_free)
{
	if (context == nullptr)
		throw SSLError(fmt::format("Could not create SSL context. {}", sslErrorString()));

	if (SSL_CTX_use_certificate_file(context.get(), cert.string().c_str(), SSL_FILETYPE_PEM) != 1)
		throw SSLError(fmt::format("Invalid certificate. {}", sslErrorString()));

	if (SSL_CTX_use_PrivateKey_file(context.get(), key.string().c_str(), SSL_FILETYPE_PEM) != 1)
		throw SSLError(fmt::format("Invalid private key. {}", sslErrorString()));

	ssl.reset(SSL_new(context.get()));

	if (ssl == nullptr)
		throw SSLError(fmt::format("Could not create ssl object. {}", sslErrorString()));

	if (SSL_set_fd(ssl.get(), static_cast<NativeSocket>(socket)) != 1)
		throw SSLError(fmt::format("Could not set file descriptor. {}", sslErrorString()));

	if (SSL_accept(ssl.get()) != 1)
		throw SSLError(fmt::format("Could not accept secure connection. {}", sslErrorString()));

	socket = std::move(socket);
}

SecureWrapper::SecureWrapper(SecureWrapper &&other) noexcept : context(nullptr, SSL_CTX_free), ssl(nullptr, SSL_free)
{
	std::swap(ssl, other.ssl);
	std::swap(context, other.context);
	std::swap(socket, other.socket);
}

SecureWrapper &SecureWrapper::operator=(SecureWrapper &&other) noexcept
{
	close();
	std::swap(ssl, other.ssl);
	std::swap(context, other.context);
	std::swap(socket, other.socket);

	return *this;
}

void SecureWrapper::close()
{
	ssl = {};
	context = {};
	socket = {};
}

} // namespace tomato
