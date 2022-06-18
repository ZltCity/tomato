#pragma once

#include <array>
#include <cstddef>

namespace tomato
{

bool checkTLS(const std::array<std::byte, 4> &signature);

}
