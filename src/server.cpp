#include <iostream>
#include <iomanip>
#include <utility>

#include "logger.h"
#include "server.hpp"
#include "utils/is_valid_config.hpp"
#include "utils/run_io_context.hpp"

namespace proxy_server_6
{

server::server( server::config_t config )
    : m_threads()
    , m_io_context()
    , m_io_context_work()
    , m_host( std::move( config.host ) )
    , m_port( std::move( config.port ) )
    , m_dir_path( std::move( config.dir_path ) )
    , m_maxdata_size( config.maxdata_size )
    , m_workers_count( config.workers_count )
    , m_verbose( config.verbose )
{
    // setw(10) - length "dir_path"
    LOG_IF( m_verbose, std::cout, "proxy_server::server::" <<  __FUNCTION__ 
            << "() - success with config:\n"
            << std::left << std::setw( 10 ) << "ip: "       << m_host                      << ",\n"
            << std::left << std::setw( 10 ) << "port: "     << m_port                      << ",\n"
            << std::left << std::setw( 10 ) << "dir_path: " << m_dir_path                  << ",\n"
            << std::left << std::setw( 10 ) << "maxdata: "  << m_maxdata_size              << ",\n"
            << std::left << std::setw( 10 ) << "workers: "  << m_workers_count             << ",\n"
            << std::left << std::setw( 10 ) << "verbose: "  << std::boolalpha << m_verbose
    );
}


server::~server()
{
    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "() - BEGIN"
    );

    stop();

    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "() - END"
    );
}


void server::start()
{
    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "()"
    );

    m_io_context_work.emplace( boost::asio::make_work_guard( m_io_context ) );
    for( int64_t i = 0; i < m_workers_count; ++i )
    {
        m_threads.create_thread( proxy_server_6::run_io_context( m_io_context ) );
    }
}


void server::stop()
{
    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "() - BEGIN"
    );

    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "() - stop io_context"
    );

    m_io_context.stop();

    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "() - join threads"
    );

    m_threads.join_all();

    LOG_IF( m_verbose, std::cout, "proxy_server::server::" << __FUNCTION__
            << "() - END"
    );
}


bool is_valid_config( proxy_server_6::server::config_t & config )
{
    return  proxy_server_6::is_valid_host( config.host )                    &&
            proxy_server_6::is_valid_port( config.port )                    &&
            proxy_server_6::is_valid_dir_path( config.dir_path )            &&
            proxy_server_6::is_valid_maxdata_size( config.maxdata_size )    &&
            proxy_server_6::is_valid_workers_count( config.workers_count );
}

}   // namespace proxy_server_6