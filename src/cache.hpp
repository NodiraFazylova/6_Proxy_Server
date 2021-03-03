#ifndef CACHE_HPP
#define CACHE_HPP

#include <atomic>
#include <chrono>
#include <future>
#include <iterator>
#include <iostream>
#include <mutex>
#include <map>
#include <vector>
#include <queue>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/post.hpp>

namespace proxy_server_6
{

class cache
{

public:
    explicit cache( boost::asio::io_context & io_context, size_t bucket_count = 5, std::size_t max_data_size = 1024, bool verbose = false );

    ~cache() = default;

private:
    cache( cache & other ) = delete;
    cache & operator=( cache & other ) = delete;

    cache( cache && other ) = delete;
    cache & operator=( cache && other ) = delete;

public:

    std::vector<std::string> get_cached_files();

    void insert( const std::string & command, const std::string & file );

    bool get_file( const std::string & command, std::string & file );

private:

    void delete_oldest_and_insert( const std::string & command, const std::string & file );


private:
    class proxy_server_6::command_queue;

    struct bucket
    {
        std::mutex                      m_mtx;
        std::map<size_t, std::string>   m_files_by_command;
    };


    boost::asio::io_context & m_io_context;

    std::atomic_size_t                     m_cur_size;
    size_t                                 m_max_size;
    std::vector<bucket>                    m_data;
    std::unique_ptr<command_queue> m_command_by_time; // pimpl for queue

    bool                      m_verbose;
};

}   // namespace proxy_server_6

#endif // !CACHE_HPP

