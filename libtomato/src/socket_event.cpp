#include "tomato/socket_event.hpp"

namespace tomato
{

SocketEvent operator&(SocketEvent a, SocketEvent b)
{
	return static_cast<SocketEvent>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

SocketEvent operator|(SocketEvent a, SocketEvent b)
{
	return static_cast<SocketEvent>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

} // namespace tomato
