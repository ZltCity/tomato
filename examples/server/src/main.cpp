#include <iostream>
#include <stdexcept>
#include <thread>

#include <cstdlib>

#include <fmt/format.h>

#include <tomato/secure_wrapper.hpp>
#include <tomato/socket.hpp>

#include <tomato/listener.hpp>

int main()
{
	using namespace std::chrono_literals;

	auto sslContext = tomato::SSLContext(std::getenv("TOMATO_CERT"), std::getenv("TOMATO_CERT_KEY"));
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

			auto secure = tomato::SecureWrapper(std::move(conn), sslContext);
			auto buffer = std::array<std::byte, 1024> {};
			auto received = secure.read(buffer, 10s);

			if (received > 0)
				std::cout << std::string_view(reinterpret_cast<const char *>(buffer.data()), received) << std::endl;

			const auto data = std::string {"HTTP/1.1 200 ok\r\nConnection: close\r\n"};

			buffer = {};
			std::transform(
				data.begin(), data.end(), buffer.begin(), [](char ch) -> std::byte { return std::byte(ch); });
			secure.write(buffer);
			secure.shutdown();
		}
		else
			break;
	} while (true);

	return 0;
}