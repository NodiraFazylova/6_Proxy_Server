cmake_minimum_required(VERSION 3.5)
project(yaps)
#yaps = yet another proxy-server

#source files
file(GLOB_RECURSE sources
	server.cpp
	main.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PRIVATE
	${yaps_SOURCE_DIR}
	${yaps_SOURCE_DIR}/../3rdparty
)
