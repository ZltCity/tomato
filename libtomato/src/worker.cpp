#include "tomato/threading/worker.hpp"

namespace tomato::threading
{

Worker::Context::Context(const Routine &routine)
	: thread([this, routine]() {
		  while (alive.test())
			  try
			  {
				  if (!routine())
				  {
					  alive.clear();
					  break;
				  }
			  }
			  catch (...)
			  {
				  /* TODO: add logging. */
			  }
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
