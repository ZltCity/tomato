#include "tomato/connection_queue.hpp"

namespace tomato
{

ConnectionQueue makeConnectionQueue(size_t maxSize)
{
	return std::make_shared<threading::Queue<Connection>>(maxSize);
}

} // namespace tomato
