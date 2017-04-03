cmake_minimum_required(VERSION 2.6)
project(mlang_interpreter)

SET(CMAKE_BUILD_TYPE debug)

add_executable(mlang
	vals.cpp
	runtime.cpp
	main.cpp
)