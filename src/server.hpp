#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdint.h>
#include <string>

namespace proxy_server_6
{

class server
{
public:
    struct config_t
    {
        std::string host;
        std::string port;
        std::string dir_path{"."};
        int64_t     maxdata_size = 1024;
        int64_t     workers_count = 8;
        bool        verbose = false;
    };
};

bool is_valid_config( proxy_server_6::server::config_t & config );

}   // namespace proxy_server_6

#endif /* SERVER_HPP */
