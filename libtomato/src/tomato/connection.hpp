#pragma once

#include <tuple>

#include "socket.hpp"

namespace tomato
{

using Connection = std::tuple<Socket, SocketAddress>;

}
