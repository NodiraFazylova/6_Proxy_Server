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
	${BOOST_INCLUDE}
)

if(UNIX)
	target_link_libraries(<CONFIG>_OUTPUT_NAME{PROJECT_NAME}
		pthread
	)
else()
	target_link_libraries(${PROJECT_NAME}
		# debug
		${BOOST_LIBRARY}/libboost_date_time-clangw10-mt-gd-x64-1_74.lib
		${BOOST_LIBRARY}/libboost_regex-clangw10-mt-gd-x64-1_74.lib
		${BOOST_LIBRARY}/libboost_chrono-clangw10-mt-gd-x64-1_74.lib
		${BOOST_LIBRARY}/libboost_thread-clangw10-mt-gd-x64-1_74.lib
		
		# release
		${BOOST_LIBRARY}/libboost_date_time-clangw10-mt-x64-1_74.lib
		${BOOST_LIBRARY}/libboost_regex-clangw10-mt-x64-1_74.lib
		${BOOST_LIBRARY}/libboost_chrono-clangw10-mt-x64-1_74.lib
		${BOOST_LIBRARY}/libboost_thread-clangw10-mt-x64-1_74.lib
	)
endif()

# uncomment this to enable logger
target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_ENABLE)

# Path to additional CMake module
set(CMAKE_MODULE_PATH ${Proxy_Server_6_SOURCE_DIR}/cmake/tests ${CMAKE_MODULE_PATH})

#include module for auto parsing tests
include(ParseAndAddCatchTests)
#set(PARSE_CATCH_TESTS_VERBOSE ON)

ParseAndAddCatchTests(${PROJECT_NAME})