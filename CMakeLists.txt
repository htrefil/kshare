project(kshare)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(toml11_BUILD_TEST OFF)

set(COMPILE_OPTIONS -Wall -pedantic -Werror -g)
set(THIRDPARTY_LIBRARIES tiny-aes toml11 enet sha256 stdc++fs X11)

add_subdirectory(thirdparty/aes)
add_subdirectory(thirdparty/enet)
add_subdirectory(thirdparty/toml11)
add_subdirectory(thirdparty/sha256)
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)


