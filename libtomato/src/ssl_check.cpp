#include <algorithm>

#include "tomato/ssl_check.hpp"

namespace tomato
{

bool checkTLS(const std::array<std::byte, 4> &signature)
{
	const auto versions = std::array<std::byte, 3> {std::byte {0x01}, std::byte {0x02}, std::byte {0x03}};
	const auto [type, major, minor, notUsed] = signature;

	if (type != std::byte {0x16})
		return false;
	if (major != std::byte {0x03})
		return false;
	if (std::none_of(versions.begin(), versions.end(), [&signature](auto &&v) { return v == std::get<2>(signature); }))
		return false;

	return true;
}

} // namespace tomato
