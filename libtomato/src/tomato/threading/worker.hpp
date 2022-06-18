#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace tomato::threading
{

class Worker
{
public:
	using Routine = std::function<bool()>;

	explicit Worker(const Routine &routine);
	Worker(const Worker &) = delete;
	Worker(Worker &&) = default;
	virtual ~Worker() = default;

	Worker &operator=(const Worker &) = delete;
	Worker &operator=(Worker &&) = default;

private:
	struct Context
	{
		explicit Context(const Routine &routine);
		~Context();

		std::thread thread;
		std::atomic_flag alive;
	};

	std::unique_ptr<Context> ctx;
};

} // namespace tomato::threading
