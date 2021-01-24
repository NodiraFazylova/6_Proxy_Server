cmake_minimum_required(VERSION 3.5)
project(Proxy_Server_6)

#uncomment this for enable tests
set(PROXY_SERVER_6_TESTS ON)

#compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
set(CMAKE_C_COMPILER "clang")
endif()

# Path to additional CMake module
set(CMAKE_MODULE_PATH ${Proxy_Server_6_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

#include additional Cmake module
include(BoostPaths)

add_custom_target(${PROJECT_NAME})

add_dependencies(${PROJECT_NAME} yaps)

add_subdirectory(src)

if(NOT PROXY_SERVER_6_TESTS)
	message(STATUS "Proxy_Server_6 project testing off")
else()
	add_dependencies(${PROJECT_NAME} tests)

	enable_testing()
	add_subdirectory(tests)
endif()
