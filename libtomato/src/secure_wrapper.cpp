#include "tomato/secure_wrapper.hpp"

namespace tomato
{

SecureWrapper::SecureWrapper(Socket socket, const SSLContext &sslContext)
	: socket(std::move(socket)), ssl(nullptr, SSL_free)
{
	auto nativeContext = static_cast<SSL_CTX *>(sslContext);

	ssl.reset(SSL_new(nativeContext));

	if (ssl == nullptr)
		throw SSLError(fmt::format("Could not create ssl object. {}", sslErrorString()));

	if (SSL_set_fd(ssl.get(), static_cast<NativeSocket>(this->socket)) != 1)
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

void SecureWrapper::accept()
{
	using namespace std::chrono_literals;

	auto waitForEvent = [this](SocketEvent e) {
		if ((socket.wait(e, 30s) & e) == SocketEvent::None)
			throw SocketError("Wait timeout expired.");
	};

	while (SSL_accept(ssl.get()) <= 0)
	{
		const auto err = sslError();

		switch (err)
		{
			case SSL_ERROR_WANT_READ:
			{
				waitForEvent(SocketEvent::ReadyRead);

				break;
			}
			case SSL_ERROR_WANT_WRITE:
			{
				waitForEvent(SocketEvent::ReadyWrite);

				break;
			}

			default: throw SSLError(fmt::format("Could not accept secure connection. {}", sslErrorString()));
		}
	}
}

void SecureWrapper::release()
{
	ssl = {};
	socket = {};
}

} // namespace tomato
