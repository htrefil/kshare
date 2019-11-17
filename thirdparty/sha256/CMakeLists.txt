project(sha256 C)

add_library(sha256 sha256.c)
target_include_directories(sha256 PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(sha256 PRIVATE ${PROJECT_SOURCE_DIR}/include/sha256)