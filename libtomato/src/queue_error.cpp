#include "tomato/queue_error.hpp"

namespace tomato
{

InvalidQueue::InvalidQueue() : std::runtime_error("Invalid queue.")
{
}

EmptyQueue::EmptyQueue() : std::runtime_error("Queue is empty.")
{
}

FullQueue::FullQueue() : std::runtime_error("Queue is full.")
{
}

} // namespace tomato
