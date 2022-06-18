#include "tomato/listener.hpp"

namespace tomato
{

const uint32_t Listener::defaultTimeout;

Listener::Context::Context(const SocketAddress &bindAddress, ConnectionQueue queue)
	: socket {bindAddress.addressFamily, SocketType::Stream}, queue(std::move(queue))
{
	socket.bind(bindAddress);
	socket.listen();
}

Listener::Listener(const SocketAddress &bindAddress, ConnectionQueue queue)
	: Worker(std::bind(
		  [](std::shared_ptr<Context> ctx) -> bool {
			  auto connection = Connection {};
			  auto &[clientSocket, clientAddress] = connection;

			  clientSocket = ctx->socket.accept(clientAddress, std::chrono::milliseconds {defaultTimeout});

			  if (clientSocket != Socket {})
				  try
				  {
					  ctx->queue->push(std::move(connection));
				  }
				  catch (const InvalidQueue &)
				  {
					  return false;
				  }

			  return true;
		  },
		  std::make_shared<Context>(bindAddress, std::move(queue))))
{
}

} // namespace tomato
