#ifndef CACHE_HPP
#define CACHE_HPP

#include <atomic>
#include <chrono>
#include <future>
#include <iterator>
#include <iostream>
#include <mutex>
#include <map>
#include <set>
#include <queue>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/post.hpp>

#include "logger.h"
#include "utils/cache_locker.hpp"

namespace proxy_server_6
{

namespace detail
{
class command_queue
{
public:
    struct timed_command
    {
        std::chrono::system_clock::time_point time;
        size_t                                command_hash;
    };

    command_queue() = default;

    auto find_by_command(const size_t command_hash)
    {
        auto pos = std::begin(m_queue);
        for( auto end_pos = std::end(m_queue); pos != end_pos; pos = std::next(pos) )
        {
            if(pos->command_hash == command_hash)
            {
                break;
            }
        }

        return pos;
    }

    void insert(timed_command command)
    {
        m_queue.emplace(std::move(command));
    }

    std::mutex & get_mutex()
    {
        return m_mtx;
    }

    bool empty()
    {
        return m_queue.empty();
    }

    size_t get_front_command()
    {
        return m_queue.begin()->command_hash;
    }

    void delete_front_command()
    {
        m_queue.erase(m_queue.begin());
    }

private:

    std::mutex              m_mtx;
    std::set<timed_command> m_queue;
};

inline
bool operator< (const command_queue::timed_command & lh, const command_queue::timed_command & rh)
{
    return  (lh.time < rh.time) ||
            ((lh.time == rh.time) && (lh.command_hash < rh.command_hash));
}

} // namespace detail

class cache
{

public:
    explicit cache( boost::asio::io_context & io_context, size_t bucket_count = 5, std::size_t max_data_size = 1024, bool verbose = false )
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
            proxy_server_6::cache_locker data_locker{ m_data[index].m_mtx };
            proxy_server_6::cache_locker queue_locker{ m_command_by_time.get_mutex() };
            if( data_locker && queue_locker )
            {
                m_command_by_time.insert( { std::chrono::system_clock::now(), command_hash } ); 
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
        std::chrono::system_clock::time_point min_time = std::chrono::system_clock::now();
        proxy_server_6::cache_locker queue_locker_{ m_command_by_time.get_mutex() };
        size_t oldest_command;
        if(queue_locker_ && m_command_by_time.empty())
        {
            oldest_command = m_command_by_time.get_front_command();
            m_command_by_time.delete_front_command();

            proxy_server_6::cache_locker data_locker{m_data[oldest_command].m_mtx};
            auto & bucket = m_data[oldest_command];
            bucket.m_files_by_command.erase(oldest_command);

            // insert new value
            size_t command_hash = std::hash<std::string>{}(command);
            size_t index = command_hash % m_data.size();
            m_command_by_time.insert( { std::chrono::system_clock::now(), command_hash } );
            bucket.m_files_by_command.insert( { command_hash, file } );
            m_cur_size.store( m_cur_size.load( std::memory_order_acquire ) + file.size() );
        }        
    }


private:
    

    struct bucket
    {
        std::mutex                      m_mtx;
        std::map<size_t, std::string>   m_files_by_command;
    };


    boost::asio::io_context & m_io_context;

    std::atomic_size_t        m_cur_size;
    size_t                    m_max_size;
    std::vector<bucket>       m_data;
    detail::command_queue     m_command_by_time;

    bool                      m_verbose;
};

}   // namespace proxy_server_6

#endif // !CACHE_HPP

