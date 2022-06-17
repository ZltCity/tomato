#pragma once

#include "connection.hpp"
#include "threading/queue.hpp"

namespace tomato
{

using ConnectionQueue = std::shared_ptr<threading::Queue<Connection>>;

ConnectionQueue makeConnectionQueue(size_t maxSize = {});

} // namespace tomato
