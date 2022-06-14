#pragma once

#include <concepts>

namespace tomato
{

template<std::integral T>
class AutoCounter
{
public:
	explicit AutoCounter(T &counter);
	AutoCounter(const AutoCounter &) = delete;
	~AutoCounter();

	AutoCounter &operator=(const AutoCounter &) = delete;

private:
	T &counter;
};

template<std::integral T>
AutoCounter<T>::AutoCounter(T &counter) : counter(++counter)
{
}

template<std::integral T>
AutoCounter<T>::~AutoCounter()
{
	--counter;
}

} // namespace tomato
