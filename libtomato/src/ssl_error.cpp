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

std::string sslErrorString()
{
	auto message = std::string(256, 0u);

	ERR_error_string(ERR_get_error(), message.data());

	return message;
}

} // namespace tomato
