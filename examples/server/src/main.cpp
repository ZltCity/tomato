#include <iostream>
#include <stdexcept>
#include <thread>

#include <fmt/format.h>

#include <tomato/secure_wrapper.hpp>
#include <tomato/socket.hpp>

int main()
{
	using namespace std::chrono_literals;

	auto sock = tomato::Socket {tomato::AddressFamily::Inet, tomato::SocketType::Stream};
	auto hostAddr = tomato::SocketAddress {tomato::AddressFamily::Inet, "127.0.0.1", 443};

	sock.bind(hostAddr);
	sock.listen();

	auto conn = tomato::Socket {};
	auto connAddress = tomato::SocketAddress {};

	do
	{
		conn = sock.accept(connAddress, 30s);

		if (conn != tomato::Socket {})
		{
			std::cout << fmt::format("address: {}, port: {}", connAddress.address, connAddress.port) << std::endl;

			auto secure = tomato::SecureWrapper(
				std::move(conn), "C:/msys64/home/Roman/ca/server.crt", "C:/msys64/home/Roman/ca/server.key");
			//			auto buffer = std::array<std::byte, 1024> {};
			//			auto received = secure.receive(buffer, 10s);
			//
			//			if (received > 0)
			//				std::cout << std::string_view(reinterpret_cast<const char *>(buffer.data()), received) <<
			// std::endl;
		}
		else
			break;
	} while (true);

	//	auto conn = SOCKET {};
	//
	//	while ((conn = accept(s, &inAddr, &addreLen)) != INVALID_SOCKET)
	//	{
	//		char buffer[256] = {};
	//
	//		while (recv(conn, buffer, sizeof(buffer), 0) != SOCKET_ERROR)
	//		{
	//			const char *gg = buffer;
	//			std::cout << buffer << std::endl;
	//			memset(buffer, 0, sizeof(buffer));
	//		}
	//
	//		closesocket(conn);
	//		std::cout << "Connection closed" << std::endl;
	//	}
	//
	//	closesocket(s);
	//
	//	WSACleanup();

	return 0;
}