#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <variant>
#include <vector>

namespace protocol
{

struct connect_request
{
    std::string    host;                    // remote server host
    unsigned short port = 0;                // remote server port 
};


struct connect_response
{
    unsigned short port;                    // new port for client
};


struct disconnect_notify
{

};


struct get_cached_files_request
{

};


struct get_cached_files_response
{
    std::vector<std::string> file_paths;    // paths to cached files
};

struct get_file_request
{
    std::string file_path;                  // paths file
};


struct get_file_response
{
    std::string file_path;                  // path to requested file
};


struct reconnect_notify
{
    std::string    host;                    // remote server host
    unsigned short port = 0;                // remote server port
};


std::string make( const protocol::connect_request & packet );
std::string make( const protocol::connect_response & packet );
std::string make( const protocol::disconnect_notify & packet );
std::string make( const protocol::get_cached_files_request & packet );
std::string make( const protocol::get_cached_files_response & packet );
std::string make( const protocol::get_file_request & packet );
std::string make( const protocol::get_file_response & packet );
std::string make( const protocol::reconnect_notify & packet );


using command_variant = std::variant <
                            std::monostate,
                            protocol::connect_request,
                            protocol::connect_response,
                            protocol::disconnect_notify,
                            protocol::get_cached_files_request,
                            protocol::get_cached_files_response,
                            protocol::get_file_request,
                            protocol::get_file_response,
                            protocol::reconnect_notify>;

command_variant parse( const std::string & s );

}   // namespace protocol

#endif /* !PROTOCOL_HPP */
