cmake_minimum_required(VERSION 2.6)
project(MLang_Interpreter)

SET(CMAKE_BUILD_TYPE debug)

add_executable(mlang
	main.cpp
	runtime.cpp
)