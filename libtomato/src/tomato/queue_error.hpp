#pragma once

#include <stdexcept>

namespace tomato
{

class InvalidQueue : public std::runtime_error
{
public:
	InvalidQueue();
};

class EmptyQueue : public std::runtime_error
{
public:
	EmptyQueue();
};

class FullQueue : public std::runtime_error
{
public:
	FullQueue();
};

} // namespace tomato
