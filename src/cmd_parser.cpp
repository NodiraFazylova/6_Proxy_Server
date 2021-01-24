#include <iostream>
#include <algorithm>
#include <iomanip>
#include <iterator>
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
    parser.addArgument( { "-i", "--ip" }, &config.host, "IP address of server listener" );
    parser.addArgument( { "-p", "--port" }, &config.port, "Port of server listener" );
    parser.addArgument( { "-m", "--maxdata" }, &config.maxdata_size, "The amount of data that the server can store per client" );
    parser.addArgument( { "-w", "--workers" }, &config.workers_count, "Maximum number of available cache tables" );
    parser.addArgument( { "-v", "--verbose" }, &config.verbose, "Flag that indicates that debug messages is printed to stdout (stderr), if not set server prints only errors" );
    parser.addArgument( { "-h", "?", "--help" }, &help_opt, "Print help string" );

    return parser;
}

}   // namespace proxy_server_6