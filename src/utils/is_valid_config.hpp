#ifndef IS_VALID_CONFIG_HPP
#define IS_VALID_CONFIG_HPP

#include <stdint.h>
#include <string>

namespace proxy_server_6
{

bool is_valid_host( const std::string & host )
{
	return !host.empty();
}

bool is_valid_port( const std::string & port )
{
	return !port.empty();
}

bool is_valid_dir_path( const std::string & dir_path )
{
	return !dir_path.empty();
}

bool is_valid_maxdata_size( const int64_t maxdata_count )
{
	return maxdata_count > 0;
}

bool is_valid_workers_count( const int64_t workers_count )
{
	return workers_count > 0;
}

}	// namespace proxy_server_6

#endif	/* IS_VALID_CONFIG_HPP */