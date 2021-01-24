#include "logger.h"
#include "server.hpp"
#include "utils/is_valid_config.hpp"

namespace proxy_server_6
{

bool is_valid_config( proxy_server_6::server::config_t & config )
{
    return  proxy_server_6::is_valid_host( config.host )                    &&
            proxy_server_6::is_valid_port( config.port )                    &&
            proxy_server_6::is_valid_dir_path( config.dir_path )            &&
            proxy_server_6::is_valid_maxdata_size( config.maxdata_size )    &&
            proxy_server_6::is_valid_workers_count( config.workers_count );
}

}   // namespace proxy_server_6