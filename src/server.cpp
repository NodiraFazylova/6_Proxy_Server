#include <iostream>
#include <iomanip>
#include <utility>

#include "cache.hpp"
#include "logger/logger.hpp"
#include "server.hpp"
#include "utils/is_valid_config.hpp"
#include "utils/run_io_context.hpp"

namespace proxy_server_6
{

server::server( server::config_t config )
    : m_threads()
    , m_io_context()
    , m_io_context_work()
    , m_cache( /** default bucken cnt */5, config.maxdata_size, config.verbose)
    , m_host( std::move( config.host ) )
    , m_port( std::move( config.port ) )
    , m_dir_path( std::move( config.dir_path ) )
    , m_workers_count( config.workers_count )
    , m_verbose( config.verbose )
{
    logger::init_console_logger();

    LOG_TRACE_IF( m_verbose, "proxy_server::server::server() - success with config:\t"
                  "ip: {0}, port: {1}, dir_path: {2}, maxdata: {3}, workers: {4}, verbose: {5}",
                  m_host,
                  m_port,
                  m_dir_path,
                  config.maxdata_size,
                  m_workers_count,
                  m_verbose
    );
}


server::~server()
{
    LOG_TRACE_IF( m_verbose, "proxy_server::server::~server() - BEGIN" );

    stop();

    LOG_TRACE_IF( m_verbose, "proxy_server::server::~server() - END" );

    logger::deinit_logger();
}


void server::start()
{
    LOG_TRACE_IF( m_verbose, "proxy_server::server::start()" );

    m_io_context_work.emplace( boost::asio::make_work_guard( m_io_context ) );
    for( int64_t i = 0; i < m_workers_count; ++i )
    {
        m_threads.create_thread( proxy_server_6::run_io_context( m_io_context ) );
    }
}


void server::stop()
{
    LOG_TRACE_IF( m_verbose, "proxy_server::server::stop() - BEGIN" );

    LOG_TRACE_IF( m_verbose, "proxy_server::server::stop() - stop io_context" );

    m_io_context.stop();

    LOG_TRACE_IF( m_verbose, "proxy_server::server::stop() - join threads" );

    m_threads.join_all();

    LOG_TRACE_IF( m_verbose, "proxy_server::server::stop() - END" );
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