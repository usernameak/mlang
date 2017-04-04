cmake_minimum_required(VERSION 2.6)
project(mlang_interpreter)

add_library(mlang_shared SHARED
	vals.cpp
	runtime.cpp
)

set_target_properties(mlang_shared
	PROPERTIES OUTPUT_NAME mlang)

add_executable(mlang
	main.cpp
)

target_link_libraries(mlang mlang_shared)