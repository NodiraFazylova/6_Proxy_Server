#ifndef CMD_LINE_PARSER_HPP
#define CMD_LINE_PARSER_HPP

#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "server.hpp"

/**
 * Slightly adapted command line parser
 * http://schneegans.github.io/tutorials/2019/08/06/commandline 
 */

namespace proxy_server_6
{

#include "cmd_parser/CommandLine.hpp"

using cmd_parser = CommandLine;

cmd_parser make_cmd_parser( proxy_server_6::server::config_t & config, bool & help_opt);

}   // namespace proxy_server_6

#endif  /* CMD_LINE_PARSER */