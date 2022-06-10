cmake_minimum_required(VERSION 3.20)

project(Tomato
	C
	CXX)

include(FetchContent)

if (MSVC)
	add_compile_options(/W4 /wd4201)
elseif (GNU OR Clang)
	add_compile_options(-Wall -Wextra -Wpedantic -Wno-gnu-anonymous-struct -Wno-nested-anon-types)
endif ()

add_subdirectory(libtomato)
add_subdirectory(contrib/json)
add_subdirectory(contrib/format)
add_subdirectory(examples/server)
