#if defined(__linux__)
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include "tomato/socket_address.hpp"

namespace tomato
{

SocketAddress::SocketAddress(const sockaddr_storage &sas) : addressFamily {}, port {}
{
	switch (sas.ss_family)
	{
		case AF_INET:
		{
			const auto &sa4 = reinterpret_cast<const sockaddr_in &>(sas);

			addressFamily = AddressFamily::Inet;
			address = ::inet_ntoa(sa4.sin_addr);
			port = ::ntohs(sa4.sin_port);

			break;
		}
		case AF_INET6:
		{
			addressFamily = AddressFamily::Inet6;

			break;
		}
	}
}

SocketAddress::SocketAddress(AddressFamily addressFamily, const std::string &address, uint16_t port)
	: addressFamily(addressFamily), address(address), port(port)
{
}

SocketAddress::operator sockaddr_storage() const
{
	auto sas = sockaddr_storage {};

	switch (addressFamily)
	{
		case AddressFamily::Inet:
		{
			auto &sa4 = reinterpret_cast<sockaddr_in &>(sas);

#if defined(_WIN32)
			sa4.sin_family = static_cast<short>(addressFamily);
			sa4.sin_addr.S_un.S_addr = ::inet_addr(address.c_str());
#elif defined(__linux__)
			sa4.sin_family = static_cast<sa_family_t>(addressFamily);
			sa4.sin_addr.s_addr = static_cast<in_addr_t>(::inet_addr(address.c_str()));
#endif
			sa4.sin_port = ::htons(port);

			break;
		}
		case AddressFamily::Inet6:
		{
			break;
		}
	}

	return sas;
}

} // namespace tomato
