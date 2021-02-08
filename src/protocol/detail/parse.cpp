#include <iostream>
#include "string"

#include "logger.h"
#include "protocol/protocol.hpp"


namespace protocol
{

protocol::command_variant parse( const std::string & s )
{
    try
    { 
        auto command_end_pos = s.find_last_of( ' ' );
        std::string command = s.substr( 0, command_end_pos );
        if( command == "connect to" )
        {
            auto host_end_pos = s.find( ':' );
            if( command_end_pos != s.npos )
            {
                protocol::connect_request v;

                v.host = s.substr( command_end_pos + 1, host_end_pos - command_end_pos - 1 );
                if( host_end_pos != s.npos )
                {
                    v.port = static_cast<unsigned short>(std::stoi( s.substr( host_end_pos + 1 ) ));
                }

                if( v.port != 0 ) // we can-t use port == 0;
                {
                    return v;
                }
            }
        }
        else if( command == "disconnect" )
        {
            protocol::disconnect_notify v;

            return v;
        }
        else if( command == "get list of cached files" )
        {
            protocol::get_cached_files_request v;

            return v;
        }
        else if( command == "list of cached files" )
        {
            if( command_end_pos != s.npos )
            {
                protocol::get_cached_files_response v;

                std::string_view file_paths = s.substr( command_end_pos + 1 );

                for( auto end_pos = file_paths.find_first_of( '\n' );
                     end_pos != file_paths.npos;
                     end_pos = file_paths.find_first_of( '\n' ) )
                {
                    v.file_paths.emplace_back( file_paths.substr( 0, end_pos ) );
                    file_paths = file_paths.substr( end_pos + 1 );
                }

                if( !file_paths.empty() )
                {
                    v.file_paths.emplace_back( file_paths );
                }

                return v;
            }
        }
        else if( command == "get file" )
        {
            if( command_end_pos != s.npos )
            {
                protocol::get_file_request v;

                v.file_path = s.substr( command_end_pos + 1 );

                if( !v.file_path.empty() )
                {
                    return v;
                }
            }
        }
        else if( command == " file" )
        {
            protocol::get_file_response v;
            if( command_end_pos != s.npos )
            {
                v.file_path = s.substr( command_end_pos + 1 );
            }

            return v;
        }
        else if( command == "reconnect to" )
        {
            auto host_end_pos = s.find( ':' );
            if( command_end_pos != s.npos )
            {
                protocol::reconnect_notify v;

                v.host = s.substr( command_end_pos + 1, host_end_pos );
                if( host_end_pos != s.npos )
                {
                    v.port = static_cast<unsigned short>(std::stoi( s.substr( host_end_pos - command_end_pos - 1 ) ) );
                }

                if( v.port != 0 ) // we can-t use port == 0;
                {
                    return v;
                }
            }
        }
    }
    catch( const std::exception & e )
    {
        LOG_ERROR( std::cerr, e.what() );
    }
    catch( ... )
    {
        LOG_ERROR( std::cerr, "unknown error in protocol::parse()" );
    }
    

    return {}; // std::monostate as value
}

}   // namespace protocol