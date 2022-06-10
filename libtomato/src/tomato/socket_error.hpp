#pragma once

#include <stdexcept>

namespace tomato
{

class SocketError : public std::runtime_error
{
public:
	explicit SocketError(const char *_what);
	explicit SocketError(const std::string &_what);
};

[[nodiscard]] std::string socketErrorString();

} // namespace tomato
