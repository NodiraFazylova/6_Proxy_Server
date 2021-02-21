#ifndef CACHE_HPP
#define CACHE_HPP

#include <atomic>
#include <chrono>
#include <future>
#include <iterator>
#include <iostream>
#include <mutex>
#include <map>
#include <queue>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/post.hpp>

#include "logger.h"
#include "utils/cache_locker.hpp"

namespace proxy_server_6
{

class cache
{

public:
    explicit cache( boost::asio::io_context & io_context, size_t bucket_count = 5, std::size_t max_data_size = 1024, bool verbose = false )
        : m_io_context( io_context )
        , m_cur_size( 0 )
        , m_max_size( max_data_size )
        , m_data( bucket_count )
        , m_verbose( verbose )
    {
        LOG_IF( m_verbose,
                std::cout,
                "cache::cache()"
        );
    }

    ~cache() = default;

private:
    cache( cache & other ) = delete;
    cache & operator=( cache & other ) = delete;

    cache( cache && other ) = delete;
    cache & operator=( cache && other ) = delete;

public:

    std::vector<std::string> get_cached_files()
    {
        LOG_IF( m_verbose,
                std::cout,
                "cache::get_cached_files() - BEGIN"
        );

        std::vector<std::string> result;

        for( auto & bckt : m_data )
        {
            LOG_IF( m_verbose,
                    std::cout,
                    "cache::get_cached_files() - add next bucket to result"
            );
            /* cache_locker does not necessarily block the desired mutex, and here we need a guaranteed block, so we use std::lock_guard */
            std::lock_guard cache_guard( bckt.m_mtx );
            for( const auto & [key, value] : bckt.m_files_by_command )
            {
                result.emplace_back( value );
            }
        }

        LOG_IF( m_verbose,
                std::cout,
                "cache::get_cached_files() - END"
        );

        return result;
    }

    void insert( const std::string & command, const std::string & file )
    {
        size_t command_hash = std::hash<std::string>{}(command);
        size_t index        = command_hash % m_data.size();

        if( m_cur_size.load( std::memory_order_acquire ) + file.size() > m_max_size )
        {
            proxy_server_6::cache_locker locker{ m_data[index].m_mtx };
            if( locker )
            {
                m_data[index].m_command_by_time.push( { std::chrono::system_clock::now(), command_hash } );
                m_data[index].m_files_by_command.insert( { command_hash, file } );
                m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) + file.size() );
            }
            else
            {
                m_io_context.post( std::bind( &cache::insert, this, command, file ) );
            }
        }
        else
        {
            m_io_context.post( std::bind( &cache::delete_oldest_and_insert, this, command, file ) );
        }       
    }

    bool get_file( const std::string & command, std::string & file )
    {
        bool result = false;
        size_t command_hash = std::hash<std::string>{}(command);
        size_t index = command_hash % m_data.size();

        proxy_server_6::cache_locker locker{ m_data[index].m_mtx };
        if( locker )
        {
            file = m_data[index].m_files_by_command[command_hash];
            result = true;
        }
        
        return result;
    }

private:

    void delete_oldest_and_insert( const std::string & command, const std::string & file )
    {
        //  here we need a guaranteed block, so we use std::lock_guar
        std::vector<std::lock_guard<std::mutex>> lockers;
        size_t min_index = 0;
        std::chrono::system_clock::time_point min_time = std::chrono::system_clock::now();
        for( size_t index = 0; index < m_data.size(); ++index )
        {
            if( !m_data[index].m_command_by_time.empty() )
            {
                auto t_command = m_data[index].m_command_by_time.front();
                min_time       = std::min( min_time, t_command.time );
                min_index      = index;
            }
        }

        auto t_command = m_data[min_index].m_command_by_time.front();
        m_data[min_index].m_command_by_time.pop();

        m_data[min_index].m_files_by_command.erase( t_command.command_hash );

        // insert new value
        size_t command_hash = std::hash<std::string>{}(command);
        size_t index = command_hash % m_data.size();

        m_data[index].m_command_by_time.push( { std::chrono::system_clock::now(), command_hash } );
        m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) + file.size() );
    }


private:

    struct timed_command
    {
        std::chrono::system_clock::time_point time;
        size_t                                command_hash;
    };

    struct bucket
    {
        std::mutex                      m_mtx;
        std::queue<timed_command>       m_command_by_time;
        std::map<size_t, std::string>   m_files_by_command;
    };


    boost::asio::io_context & m_io_context;

    std::atomic_size_t        m_cur_size;
    size_t                    m_max_size;
    std::vector<bucket>       m_data;

    bool                      m_verbose;
};

}   // namespace proxy_server_6

#endif // !CACHE_HPP

