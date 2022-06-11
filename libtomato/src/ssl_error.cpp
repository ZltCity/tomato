#include <openssl/err.h>

#include "tomato/ssl_error.hpp"

namespace tomato
{

SSLError::SSLError(const char *_what) : std::runtime_error(_what)
{
}

SSLError::SSLError(const std::string &_what) : std::runtime_error(_what)
{
}

unsigned long sslError()
{
	return ERR_get_error();
}

std::string sslErrorString()
{
	return sslErrorString(ERR_get_error());
}

std::string sslErrorString(unsigned long err)
{
	auto message = std::string(256, 0u);

	ERR_error_string(err, message.data());

	return message;
}

void sslClearErrorStack()
{
	while (ERR_get_error() != 0)
		;
}

} // namespace tomato
