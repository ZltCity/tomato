#pragma once

#include <cstddef>

#include <chrono>

namespace tomato
{

class Pipe
{
public:
	virtual ~Pipe() = 0;

	virtual size_t read(std::byte *buffer, size_t length, std::chrono::milliseconds timeout, bool peek) = 0;
	virtual size_t write(const std::byte *buffer, size_t length, std::chrono::milliseconds timeout) = 0;
};

} // namespace tomato
