#include <iostream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <variant>
#include <vector>

#include "logger.h"
#include "cmd_parser.hpp"
#include "server.hpp"

namespace proxy_server_6
{

#include "cmd_parser/CommandLine.ipp"

cmd_parser make_cmd_parser( proxy_server_6::server::config_t & config, bool & help_opt )
{
    cmd_parser parser;
    parser.addArgument( { "-d", "--dir" }, &config.dir_path, "Path to the directory where files will be stored" );
    parser.addArgument( { "-i", "--ip" }, &config.dir_path, "IP address of server listener" );
    parser.addArgument( { "-p", "--port" }, &config.dir_path, "Port of server listener" );
    parser.addArgument( { "-m", "--maxdata" }, &config.dir_path, "The amount of data that the server can store per client" );
    parser.addArgument( { "-w", "--workers" }, &config.dir_path, "Maximum number of available cache tables" );
    parser.addArgument( { "-v", "--verbose" }, &config.dir_path, "Flag that indicates that debug messages is printed to stdout (stderr), if not set server prints only errors" );
    parser.addArgument( { "-h", "?", "--help" }, &config.dir_path, "Print help string" );

    return parser;
}

}   // namespace proxy_server_6