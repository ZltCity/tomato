#pragma once

#include <stdexcept>
#include <string>

namespace tomato
{

class SSLError : public std::runtime_error
{
public:
	explicit SSLError(const char *_what);
	explicit SSLError(const std::string &_what);
};

[[nodiscard]] unsigned long sslError();
[[nodiscard]] std::string sslErrorString();
[[nodiscard]] std::string sslErrorString(unsigned long err);

void sslClearErrorStack();

} // namespace tomato
