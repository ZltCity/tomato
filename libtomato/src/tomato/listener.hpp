#pragma once

#include "connection_queue.hpp"
#include "threading/worker.hpp"

namespace tomato
{

class Listener final : public threading::Worker
{
public:
	Listener(const SocketAddress &bindAddress, ConnectionQueue queue);
	Listener(const Listener &) = delete;
	Listener(Listener &&other) noexcept = default;

	Listener &operator=(const Listener &) = delete;
	Listener &operator=(Listener &&other) noexcept = default;

	static const uint32_t defaultTimeout = 30; // ms

private:
	struct Context
	{
		Context(const SocketAddress &bindAddress, ConnectionQueue queue);

		Socket socket;
		ConnectionQueue queue;
	};
};

} // namespace tomato
