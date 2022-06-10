set(OPENSSL_USE_STATIC_LIBS)

find_package(OpenSSL REQUIRED)

if (UNIX)
elseif (WIN32)
	set(PLATFORM_LIBRARIES
		Ws2_32.lib)
endif ()

add_library(libtomato STATIC
	src/secure_wrapper.cpp
	src/socket.cpp
	src/socket_address.cpp
	src/socket_error.cpp
	src/ssl_error.cpp)

set_target_properties(libtomato PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON)
target_include_directories(libtomato PUBLIC
	src)
target_link_libraries(libtomato PRIVATE
	fmt::fmt-header-only
	nlohmann_json::nlohmann_json
	OpenSSL::SSL
	${PLATFORM_LIBRARIES})
