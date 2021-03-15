#include <atomic>
#include <chrono>
#include <iterator>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/post.hpp>

#include "cache.hpp"
#include "detail/bucket.hpp"
#include "detail/command_queue.hpp"
#include "logger.h"

namespace proxy_server_6
{

class cache::cache_impl
{

public:
    
    explicit cache_impl( boost::asio::io_context & io_context, size_t bucket_count = 5, std::size_t max_data_size = 1024, bool verbose = false )
        : m_io_context( io_context )
        , m_cur_size( 0 )
        , m_max_size( max_data_size )
        , m_data( bucket_count )
        , m_command_by_time()
        , m_verbose( verbose )
    {
        LOG_IF( m_verbose,
                std::cout,
                "cache::cache()"
        );
    }


    std::vector<std::string> get_cached_files()
    {
        LOG_IF( m_verbose,
                std::cout,
                "cache::get_cached_files() - BEGIN"
        );

        std::vector<std::string> result;

        for( auto & bucket : m_data )
        {
            LOG_IF( m_verbose,
                    std::cout,
                    "cache::get_cached_files() - add next bucket to result"
            );

            /* cache_locker does not necessarily block the desired mutex, and here we need a guaranteed block, so we use std::lock_guard */
            std::lock_guard cache_guard( bucket.get_mutex() );
            bucket.get_all_files( result );
        }

        LOG_IF( m_verbose,
                std::cout,
                "cache::get_cached_files() - END"
        );

        return result;
    }


    void insert_file( const std::string & command, const std::string & file )
    {
        size_t command_hash = std::hash<std::string>{}(command);
        size_t index = command_hash % m_data.size();

        if( m_cur_size.load( std::memory_order_acquire ) + file.size() > m_max_size )
        {
            std::lock_guard data_locker{ m_data[index].get_mutex() };
            std::lock_guard queue_locker{ m_command_by_time.get_mutex() };

            m_command_by_time.insert( { std::chrono::system_clock::now(), command_hash } );
            m_data[index].insert( command_hash, file );
            m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) + file.size() );
        }
        else
        {
            m_io_context.post( std::bind( &cache_impl::delete_oldest_file, this, command, file ) );
        }
    }


    void delete_oldest_file( const std::string & command, const std::string & file )
    {
        std::chrono::system_clock::time_point min_time = std::chrono::system_clock::now();
        std::lock_guard queue_locker{ m_command_by_time.get_mutex() };
        
        size_t oldest_command;
        size_t index;
        if( !m_command_by_time.empty() )
        {
            oldest_command  = m_command_by_time.get_front_command();
            index           = oldest_command % m_data.size();
            m_command_by_time.delete_front_command();
        }
        
        std::lock_guard data_locker{ m_data[oldest_command].get_mutex() };
        auto & bucket     = m_data[index];
        const auto & old_file = bucket.get_file( oldest_command );

        m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) - old_file.size() );
        bucket.erase( oldest_command );

       

        m_io_context.post( std::bind( &cache_impl::insert_file, this, command, file ) );
    }


    std::string get_file( const std::string & command )
    {
        size_t command_hash = std::hash<std::string>{}(command);
        size_t index = command_hash % m_data.size();

        std::lock_guard locker{ m_data[index].get_mutex() };
        
        return m_data[index].get_file(command_hash);
    }

private:
    boost::asio::io_context                   & m_io_context;

    std::atomic_size_t                          m_cur_size;
    size_t                                      m_max_size;
    std::vector<proxy_server_6::detail::bucket> m_data;
    proxy_server_6::detail::command_queue       m_command_by_time;
    bool                                        m_verbose;
};


cache::cache( boost::asio::io_context & io_context, size_t bucket_count, std::size_t max_data_size, bool verbose )
    : m_impl( std::make_unique<cache::cache_impl>( io_context, bucket_count, max_data_size, verbose ) )
{}


cache::~cache() = default;


std::vector<std::string> cache::get_cached_files()
{
    m_impl->get_cached_files();
}


void cache::insert_file( const std::string & command, const std::string & file )
{
    m_impl->insert_file(command, file);
}


std::string cache::get_file( const std::string & command )
{
    return m_impl->get_file( command);
}

} // namespace proxy_server_6