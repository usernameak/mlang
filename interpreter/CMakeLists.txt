cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_SOURCE_DIR}/includes)

add_library(mlang_shared SHARED
	vals.cpp
	runtime.cpp
)
set_target_properties(mlang_shared
	PROPERTIES OUTPUT_NAME mlang)
target_link_libraries(mlang_shared dl)

add_executable(mlang
	main.cpp
)
target_link_libraries(mlang mlang_shared)