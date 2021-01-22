cmake_minimum_required(VERSION 3.5)
project(tests)

option(PROXY_SERVER_6_TESTS "Include unit tests" OFF)

file(GLOB_RECURSE sources
	main.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PRIVATE
	${tests_SOURCE_DIR}
	${tests_SOURCE_DIR}/../src
	${tests_SOURCE_DIR}/../3rdparty
)