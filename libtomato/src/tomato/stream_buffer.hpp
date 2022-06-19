#pragma once

#include <array>
#include <memory>
#include <streambuf>

#include "pipe.hpp"

namespace tomato
{

template<class T, size_t size = 2048>
class StreamBuffer final : public std::basic_streambuf<T>
{
public:
	explicit StreamBuffer(std::shared_ptr<Pipe> pipe);

	typename StreamBuffer::int_type underflow() final;

private:
	std::shared_ptr<Pipe> pipe;
	std::array<std::byte, size> buffer;
};

template<class T, size_t size>
StreamBuffer<T, size>::StreamBuffer(std::shared_ptr<Pipe> pipe) : pipe(std::move(pipe))
{
}

template<class T, size_t size>
auto StreamBuffer<T, size>::underflow() -> typename StreamBuffer::int_type
{
	using namespace std::chrono_literals;

	if (this->gptr() >= this->egptr())
	{
		auto received = size_t {};

		if ((received = pipe->read(buffer.data(), buffer.size(), 5s, false)) == 0)
			return StreamBuffer::traits_type::eof();

		auto *dataBegin = buffer.data();

		this->setg(dataBegin, dataBegin, dataBegin + received);
	}

	return StreamBuffer::traits_type::to_int_type(*this->gptr());
}

} // namespace tomato
