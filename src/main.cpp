#include <iostream>

#include "cmd_parser.hpp"
#include "logger/logger.hpp"
#include "server.hpp"

int main( int argc, char ** argv )
{
    proxy_server_6::server::config_t    config;
    bool                                help_opt = false;
    proxy_server_6::cmd_parser          parser = proxy_server_6::make_cmd_parser( config, help_opt );

    if( help_opt || !proxy_server_6::is_valid_config( config ) )
    {
        parser.printHelp();
        return 0;
    }

    proxy_server_6::server server( config );

    server.start();

    return 0;
}