#pragma once

#include <string>

#include "address_family.hpp"

namespace tomato
{

struct SocketAddress
{
	SocketAddress() = default;
	SocketAddress(AddressFamily addressFamily, const std::string &address, uint16_t port);
	explicit SocketAddress(const sockaddr_storage &sas);

	[[nodiscard]] explicit operator sockaddr_storage() const;

	AddressFamily addressFamily;
	std::string address;
	uint16_t port;
};

} // namespace tomato
