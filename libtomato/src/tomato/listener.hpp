#pragma once

#include <memory>
#include <tuple>

#include "socket.hpp"
#include "threading/queue.hpp"

namespace tomato
{

class Listener
{
public:
	using Connection = std::tuple<Socket, SocketAddress>;

	Listener(const SocketAddress &bindAddress, std::shared_ptr<threading::Queue<Connection>> queue);
};

} // namespace tomato
