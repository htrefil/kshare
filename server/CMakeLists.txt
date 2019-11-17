project(server CXX)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(server ${SOURCES})

target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/..)
target_link_libraries(server ${THIRDPARTY_LIBRARIES} common)
target_compile_options(server PRIVATE ${KSHARE_COMPILE_OPTIONS})