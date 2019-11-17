project(client CXX)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(client ${SOURCES})

target_include_directories(client PRIVATE ${PROJECT_SOURCE_DIR}/..)
target_link_libraries(client ${THIRDPARTY_LIBRARIES} common)
target_compile_options(client PRIVATE ${KSHARE_COMPILE_OPTIONS})
