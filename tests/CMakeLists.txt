cmake_minimum_required(VERSION 3.5)
project(tests)

option(PROXY_SERVER_6_TESTS "Include unit tests" OFF)

file(GLOB_RECURSE sources
	${tests_SOURCE_DIR}/../src/cmd_parser.cpp
	${tests_SOURCE_DIR}/../src/server.cpp
	main.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PRIVATE
	${tests_SOURCE_DIR}
	${tests_SOURCE_DIR}/../src
	${tests_SOURCE_DIR}/../3rdparty
)

# Path to additional CMake module
set(CMAKE_MODULE_PATH ${Proxy_Server_6_SOURCE_DIR}/cmake/tests ${CMAKE_MODULE_PATH})

#include module for auto parsing tests
include(ParseAndAddCatchTests)
#set(PARSE_CATCH_TESTS_VERBOSE ON)

ParseAndAddCatchTests(${PROJECT_NAME})