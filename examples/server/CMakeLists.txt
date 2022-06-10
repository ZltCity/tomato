add_executable(server
	src/main.cpp)

set_target_properties(server PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON)
target_link_libraries(server PRIVATE
	libtomato)
target_link_libraries(server PUBLIC
	fmt::fmt-header-only)
