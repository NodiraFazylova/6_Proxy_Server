######################################################################
#
# This file is part of CMake configurations for 6_Proxy_Server
#
######################################################################

# install the cmake modules
file(GLOB PROXY_SERVER_6_CMAKE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/"*.cmake
	"${CMAKE_CURRENT_SOURCE_DIR}/tests/"*.cmake
)