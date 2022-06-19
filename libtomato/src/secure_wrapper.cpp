#include "tomato/secure_wrapper.hpp"

namespace tomato
{

const uint32_t SecureWrapper::defaultTimeout;

SecureWrapper::SecureWrapper(Socket socket, const SSLContext &sslContext)
	: socket(std::move(socket)), ssl(nullptr, SSL_free)
{
	auto nativeContext = static_cast<SSL_CTX *>(sslContext);

	ssl.reset(SSL_new(nativeContext));

	if (ssl == nullptr)
		throw SSLError(fmt::format("Could not create ssl object. {}", sslErrorString()));

	const auto nativeSocket = static_cast<NativeSocket>(this->socket);

	if (SSL_set_fd(ssl.get(), static_cast<int>(nativeSocket)) != 1)
		throw SSLError(fmt::format("Could not set file descriptor. {}", sslErrorString()));

	accept();
}

SecureWrapper::SecureWrapper(SecureWrapper &&other) noexcept : ssl(nullptr, SSL_free)
{
	std::swap(socket, other.socket);
	std::swap(ssl, other.ssl);
}

SecureWrapper &SecureWrapper::operator=(SecureWrapper &&other) noexcept
{
	release();
	std::swap(socket, other.socket);
	std::swap(ssl, other.ssl);

	return *this;
}

void SecureWrapper::shutdown()
{
	sslClearErrorStack();

	auto result = int {};

	while ((result = SSL_shutdown(ssl.get())) < 0)
	{
		if (!waitSocket(SSL_get_error(ssl.get(), result)))
			throw SSLError(fmt::format("Could not shutdown SSL connection. {}", sslErrorString()));
	}
}

size_t SecureWrapper::read(std::byte *buffer, size_t length, std::chrono::milliseconds timeout, bool peek)
{
	sslClearErrorStack();

	int (*readFunction)(SSL *, void *, int) = peek ? SSL_peek : SSL_read;
	auto received = int {};

	while ((received = readFunction(ssl.get(), buffer, static_cast<int>(length))) <= 0)
	{
		if (!waitSocket(SSL_get_error(ssl.get(), received)))
			return 0;
	}

	return received;
}

size_t SecureWrapper::write(const std::byte *buffer, size_t length, std::chrono::milliseconds timeout)
{
	sslClearErrorStack();

	auto sent = int {};

	while ((sent = SSL_write(ssl.get(), buffer, static_cast<int>(length))) <= 0)
	{
		if (!waitSocket(SSL_get_error(ssl.get(), sent)))
			return 0;
	}

	return sent;
}

void SecureWrapper::accept()
{
	sslClearErrorStack();

	auto result = int {};

	while ((result = SSL_accept(ssl.get())) <= 0)
	{
		if (!waitSocket(SSL_get_error(ssl.get(), result)))
			throw SSLError(fmt::format("Could not complete SSL handshake. {}", sslErrorString()));
	}
}

void SecureWrapper::release()
{
	ssl = {};
	socket = {};
}

bool SecureWrapper::waitSocket(int retCode) const
{
	auto waitFunction = [this](SocketEvent e) -> bool {
		return (socket.wait(e, std::chrono::milliseconds {defaultTimeout}) & e) != SocketEvent::None;
	};

	switch (retCode)
	{
		case SSL_ERROR_WANT_READ: return waitFunction(SocketEvent::ReadyRead);
		case SSL_ERROR_WANT_WRITE: return waitFunction(SocketEvent::ReadyWrite);

		default: return false;
	}
}

} // namespace tomato
