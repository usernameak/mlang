cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_SOURCE_DIR}/includes)

add_library(mlang_stdlib SHARED
	io
)

target_link_libraries(mlang_stdlib mlang_shared)