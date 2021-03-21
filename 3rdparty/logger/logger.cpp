#include <memory>
#include <stdexcept>
#include <string>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/custom/rotate_file_sink_with_title.h>
#include <spdlog/spdlog.h>

#include "logger.hpp"

std::shared_ptr<spdlog::logger>                                             g_logger;
std::shared_ptr<spdlog::sinks::custom::rotating_file_sink_with_title_mt>    g_rot_sink_with_title;



namespace logger
{

spdlog::logger & logger()
{
    if( g_logger )
    {
        return *g_logger;
    }
    else
    {
        std::string exc_descr( "logs not initialised" );
        throw std::logic_error( exc_descr );
    }
}


void init_null_logger()
{
    g_logger = std::make_shared<spdlog::logger>( "yaps_logger", std::make_shared<spdlog::sinks::null_sink_mt>() );
}


void init_console_logger()
{
    spdlog::set_pattern( "[%D][%T.%e](%z) [%L] %v [%tid = %t]" );
    g_logger = spdlog::stdout_color_mt( "yaps_logger" );
    g_logger->set_level( spdlog::level::trace );
    g_logger->flush_on( spdlog::level::trace );
}


void init_rotate_logger( const std::string & path, int max_file_size, int max_files )
{
    spdlog::set_pattern( "[%D][%T.%e](%z) [%L] %v [%tid = %t]" );
    g_logger = spdlog::rotating_logger_mt( "yaps_logger", path, max_file_size, max_files );
    g_logger->set_level( spdlog::level::trace );
    g_logger->flush_on( spdlog::level::trace );
}


void init_rotate_logger_with_title( const std::string & path, int max_file_size, int max_files )
{
    spdlog::set_pattern( "[%D][%T.%e](%z) [%L] %v [%tid = %t]" );

    g_rot_sink_with_title = std::make_shared<spdlog::sinks::custom::rotating_file_sink_with_title_mt>( path, max_file_size, max_files );
    g_logger = std::make_shared<spdlog::logger>( "yaps_logger", g_rot_sink_with_title );
    spdlog::details::registry::instance().initialize_logger( g_logger );

    g_logger->set_level( spdlog::level::trace );
    g_logger->flush_on( spdlog::level::trace );
}


void set_rotate_logger( const std::string & title )
{
    auto sink = g_rot_sink_with_title;
    if( sink )
    {
        sink->set_title( title );
    }
}


void init_daily_logger( const std::string & path )
{
    spdlog::set_pattern( "[%D][%T.%e](%z) [%L] %v [%tid = %t]" );
    g_logger = spdlog::daily_logger_mt( "yaps_logger", path, 0, 0 );

    g_logger->set_level( spdlog::level::trace );
    g_logger->flush_on( spdlog::level::trace );
}


void deinit_logger()
{
    spdlog::drop_all();
    g_logger.reset();
    g_rot_sink_with_title.reset();
}

}   // namespace logger
