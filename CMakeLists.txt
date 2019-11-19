project(kshare)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(toml11_BUILD_TEST OFF)

set(COMPILE_OPTIONS -Wall -pedantic -Werror)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -g)
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
	set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -O3)
else()
	message(FATAL_ERROR "Invalid configuration")
endif()

set(THIRDPARTY_LIBRARIES tiny-aes toml11 enet sha256 stdc++fs X11)

add_subdirectory(thirdparty/aes)
add_subdirectory(thirdparty/enet)
add_subdirectory(thirdparty/toml11)
add_subdirectory(thirdparty/sha256)
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)


