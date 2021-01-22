cmake_minimum_required(VERSION 3.5)
project(yaps)
#yaps = yet another proxy-server

#source files
file(GLOB_RECURSE sources
	main.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PRIVATE
	${yaps_SOURCE_DIR}
	${yaps_SOURCE_DIR}/../3rdparty
)

# Path to additional CMake module
set(CMAKE_MODULE_PATH ${Proxy_Server_6_SOURCE_DIR}/cmake/tests ${CMAKE_MODULE_PATH})

#include module for auto parsing tests
include(ParseAndAddCatchTests)

ParseAndAddCatchTests(${PROJECT_NAME})