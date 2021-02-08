#include <string>
#include <sstream>


#include "protocol/protocol.hpp"


namespace protocol
{

std::string make( const protocol::connect_request & packet )
{
    std::stringstream ss;

    ss << "connect to " << packet.host << ":" << packet.host;

    return ss.str();
}


std::string make( const protocol::connect_response & packet )
{
    std::stringstream ss;

    ss << packet.port;

    return ss.str();
}


std::string make( const protocol::disconnect_notify & packet )
{
    std::stringstream ss;

    ss << "disconnect";

    return ss.str();
}


std::string make( const protocol::get_cached_files_request & packet )
{
    std::stringstream ss;

    ss << "get list of cached files";

    return ss.str();
}


std::string make( const protocol::get_cached_files_response & packet )
{
    std::stringstream ss;

    ss << "list of cached files: ";
    for( const auto & file_path : packet.file_paths )
    {
        ss << file_path << "\n";
    }

    return ss.str();
}


std::string make( const protocol::get_file_request & packet )
{
    std::stringstream ss;

    ss << "get file " << packet.file_path;

    return ss.str();
}


std::string make( const protocol::get_file_response & packet )
{
    std::stringstream ss;

    ss << "file " << packet.file_path;

    return ss.str();
}


std::string make( const protocol::reconnect_notify & packet )
{
    std::stringstream ss;

    ss << "reconnect to " << packet.host << ":" << packet.port;

    return ss.str();
}

}   // namespace protocol
