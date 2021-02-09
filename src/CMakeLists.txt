cmake_minimum_required(VERSION 3.5)
project(yaps)
#yaps = yet another proxy-server

#source files
file(GLOB_RECURSE sources
	server.cpp
	cmd_parser.cpp
	main.cpp
	protocol/detail/make.cpp
	protocol/detail/parse.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PRIVATE
	${yaps_SOURCE_DIR}
	${yaps_SOURCE_DIR}/../3rdparty
	${BOOST_INCLUDE}
)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PUBLIC
		${BOOST_LIBRARY}/libboost_chrono.so
		${BOOST_LIBRARY}/libboost_system.so
		${BOOST_LIBRARY}/libboost_thread.so

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

