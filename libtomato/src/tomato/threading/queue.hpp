#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

#include "tomato/queue_error.hpp"

namespace tomato::threading
{

template<class T>
class Queue
{
public:
	explicit Queue(size_t maxSize = 0);
	Queue(const Queue &) = delete;

	Queue &operator=(const Queue &) = delete;

	void push(T value, bool block = true);
	[[nodiscard]] T get(bool block = true);

	void close();

private:
	[[nodiscard]] bool isFull() const;

	std::condition_variable pushNotification, getNotification;
	std::mutex accessMutex;
	std::queue<T> queue;
	size_t maxSize;
	bool closed;
};

template<class T>
Queue<T>::Queue(size_t maxSize) : maxSize(maxSize), closed(false)
{
}

template<class T>
void Queue<T>::push(T value, bool block)
{
	auto lock = std::unique_lock {accessMutex};

	if (block)
		getNotification.wait(lock, [this]() -> bool { return closed || !isFull(); });

	if (closed)
		throw InvalidQueue();

	if (!block && isFull())
		throw FullQueue();

	queue.push(std::move(value));
	pushNotification.notify_one();
}

template<class T>
T Queue<T>::get(bool block)
{
	auto lock = std::unique_lock {accessMutex};

	if (block)
		pushNotification.wait(lock, [this]() -> bool { return closed || !queue.empty(); });

	if (queue.empty())
	{
		if (closed)
			throw InvalidQueue();
		else if (!block)
			throw EmptyQueue();
	}

	auto value = std::move(queue.front());

	queue.pop();
	getNotification.notify_one();

	return value;
}

template<class T>
void Queue<T>::close()
{
	auto lock = std::unique_lock {accessMutex};

	closed = true;
	pushNotification.notify_all();
	getNotification.notify_all();
}

template<class T>
bool Queue<T>::isFull() const
{
	return maxSize > 0 && queue.size() >= maxSize;
}

} // namespace tomato::threading
