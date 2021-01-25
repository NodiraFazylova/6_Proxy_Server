#include "cmd_parser.hpp"
#include "logger.h"
#include "server.hpp"

int main(int argc, char **argv)
{
    bool help_opt = false;
    proxy_server_6::server::config_t config;
    proxy_server_6::cmd_parser parser = proxy_server_6::make_cmd_parser(config, help_opt);

    parser.parse(argc, argv);
    if( help_opt /*|| !proxy_server_6::is_valid_config(config)*/ )
    {
        parser.printHelp();
    }

    proxy_server_6::server server;

    return 0;
}