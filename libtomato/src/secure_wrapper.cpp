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
		waitSocket(SSL_get_error(ssl.get(), result));
	}
}

void SecureWrapper::accept()
{
	sslClearErrorStack();

	auto result = int {};

	while ((result = SSL_accept(ssl.get())) <= 0)
	{
		waitSocket(SSL_get_error(ssl.get(), result));
	}
}

void SecureWrapper::release()
{
	ssl = {};
	socket = {};
}

void SecureWrapper::waitSocket(int retCode) const
{
	auto waitFunction = [this](SocketEvent e) {
		if ((socket.wait(e, std::chrono::milliseconds {defaultTimeout}) & e) == SocketEvent::None)
			throw SSLError("Could not accept secure connection. Wait timeout expired.");
	};

	switch (retCode)
	{
		case SSL_ERROR_WANT_READ:
		{
			waitFunction(SocketEvent::ReadyRead);

			break;
		}
		case SSL_ERROR_WANT_WRITE:
		{
			waitFunction(SocketEvent::ReadyWrite);

			break;
		}

		default: throw SSLError("Could not accept secure connection. Unknown return code.");
	}
}

} // namespace tomato
