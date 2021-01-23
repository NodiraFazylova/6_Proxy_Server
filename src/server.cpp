#include "logger.h"
#include "server.hpp"

namespace proxy_server_6
{

bool operator== ( const server::config_t & lh, const server::config_t & rh )
{
    return  lh.host          == rh.host             &&
            lh.port          == rh.port             &&
            lh.dir_path      == rh.dir_path         &&
            lh.maxdata_size  == rh.maxdata_size     &&
            lh.workers_count == rh.workers_count    &&
            lh.verbose       == rh.verbose;
}

}   // namespace proxy_server_6