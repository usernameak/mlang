cmake_minimum_required(VERSION 2.6)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.h
    COMMAND re2c -s ${CMAKE_CURRENT_SOURCE_DIR}/lexer.re2c -o ${CMAKE_CURRENT_BINARY_DIR}/lexer.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.re2c
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_executable(mlangc
	main.cpp
    scanner.cpp
    util.cpp
    parser.cpp
    lexer.h
)
