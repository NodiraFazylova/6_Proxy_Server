#include <atomic>
#include <chrono>
#include <functional>
#include <iterator>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <utility>

#include "cache.hpp"
#include "detail/file_map.hpp"
#include "detail/file_queue.hpp"
#include "error.hpp"
#include "logger/logger.hpp"

namespace proxy_server_6
{

class cache::cache_impl
{

public:
    
    explicit cache_impl( size_t bucket_count = 5, std::size_t max_data_size = 1024, bool verbose = false )
        : m_cur_size( 0 )
        , m_max_size( max_data_size )
        , m_data( bucket_count )
        , m_command_by_time( verbose )
        , m_verbose( verbose )
    {
        LOG_TRACE_IF( m_verbose, "cache::cache()" );

        for( auto & bucket :m_data )
        {
            bucket.set_log_mode( verbose );
        }
    }


    std::vector<std::string> get_cached_files( error::errc & errc ) const
    {
        LOG_TRACE_IF( m_verbose, "cache::get_cached_files() - BEGIN" );

        std::vector<std::string> result;

        for( auto & bucket : m_data )
        {
            LOG_TRACE_IF( m_verbose, "cache::get_cached_files() - add next bucket to result" );

            /* cache_locker does not necessarily block the desired mutex, and here we need a guaranteed block, so we use std::lock_guard */
            std::lock_guard cache_guard( bucket.get_mutex() );
            bucket.get_all_files( result );
        }

        LOG_TRACE_IF( m_verbose, "cache::get_cached_files() - END" );

        return result;
    }


    std::string get_file( const std::string & filename, error::errc & errc ) const
    {
        LOG_TRACE_IF( m_verbose, "cache::get_file() - BEGIN" );

        size_t command_hash = std::hash<std::string>{}(filename);
        size_t index = command_hash % m_data.size();

        std::lock_guard locker{ m_data[index].get_mutex() };

        LOG_TRACE_IF( m_verbose, "cache::get_file() - END" );

        return m_data[index].get_file( command_hash );
    }


    void insert_file( const std::string & filename, const std::string & file, error::errc & errc )
    {
        LOG_TRACE_IF( m_verbose, "cache::insert_file() - BEGIN" );

        LOG_TRACE_IF( m_verbose, "cache::insert_file({0})", file );

        proxy_server_6::detail::file_queue::node_t command{ std::chrono::system_clock::now(), std::hash<std::string>{}(filename) };
        std::lock_guard queue_locker{ m_command_by_time.get_mutex() };
        if( m_command_by_time.contain( command ) )
        {
            m_command_by_time.update( command );
        }
        else 
        {
            if( m_cur_size.load( std::memory_order_acquire ) + file.size() >= m_max_size )
            {
                delete_oldest_file_impl( errc );
            }

            size_t index = command.filename_hash % m_data.size();

            std::lock_guard data_locker{ m_data[index].get_mutex() };

            m_command_by_time.push( command );
            m_data[index].insert( command.filename_hash, file );
            m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) + file.size() );
        }

        LOG_TRACE_IF( m_verbose, "cache::insert_file() - END" );
    }


    void delete_oldest_file( error::errc & errc )
    {
        std::lock_guard queue_locker{ m_command_by_time.get_mutex() };
        
        delete_oldest_file_impl( errc );
    }


private:

    void delete_oldest_file_impl( error::errc & errc )
    {
        LOG_TRACE_IF( m_verbose, "cache::delete_oldest_file() - BEGIN" );

        proxy_server_6::detail::file_queue::node_t oldest_command;
        size_t index;
        if( !m_command_by_time.empty() )
        {
            oldest_command = m_command_by_time.front();
            index = oldest_command.filename_hash % m_data.size();
            m_command_by_time.pop();
        }

        std::lock_guard data_locker{ m_data[index].get_mutex() };
        auto & bucket = m_data[index];
        const auto & old_file = bucket.get_file( oldest_command.filename_hash );

        m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) - old_file.size() );
        bucket.erase( oldest_command.filename_hash );

        LOG_TRACE_IF( m_verbose, "cache::delete_oldest_file() - END" );
    }


private:

    std::atomic_size_t                              m_cur_size;
    size_t                                          m_max_size;
    std::vector<proxy_server_6::detail::file_map>   m_data;
    proxy_server_6::detail::file_queue              m_command_by_time;
    bool                                            m_verbose;
};


cache::cache( size_t bucket_count, std::size_t max_data_size, bool verbose )
    : m_impl( std::make_unique<cache::cache_impl>( bucket_count, max_data_size, verbose ) )
{}


cache::~cache() = default;


std::vector<std::string> cache::get_cached_files( error::errc & errc ) const
{
    return m_impl->get_cached_files(errc);
}


void cache::insert_file( const std::string & filename, const std::string & file, error::errc & errc )
{
    m_impl->insert_file(filename, file, errc );
}


std::string cache::get_file( const std::string & filename, error::errc & errc ) const
{
    return m_impl->get_file( filename, errc);
}

} // namespace proxy_server_6