project(common CXX)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(common ${SOURCES})

target_compile_options(common PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(common ${THIRDPARTY_LIBRARIES})