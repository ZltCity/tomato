#include "tomato/threading/worker.hpp"

namespace tomato::threading
{

Worker::Context::Context(const Routine &routine)
	: thread([this, routine]() {
		  while (alive.test())
			  routine();
	  }),
	  alive {true}
{
}

Worker::Context::~Context()
{
	alive.clear();
	thread.join();
}

Worker::Worker(const Routine &routine) : ctx {std::make_unique<Context>(routine)}
{
}

} // namespace tomato::threading
