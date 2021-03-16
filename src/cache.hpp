#ifndef CACHE_HPP
#define CACHE_HPP

#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "error.hpp"

namespace proxy_server_6
{

class cache
{

public:
    explicit cache( boost::asio::io_context & io_context, size_t bucket_count = 5, std::size_t max_data_size = 1024, bool verbose = false );

    ~cache();

private:
    cache( cache & other )             = delete;
    cache & operator=( cache & other ) = delete;

    cache( cache && other )             = delete;
    cache & operator=( cache && other ) = delete;

public:

    std::vector<std::string> get_cached_files( error::errc & errc ) const;

    void insert_file( const std::string & filename, const std::string & file, error::errc & errc );

    std::string get_file( const std::string & filename, error::errc & errc ) const;

private:
    // use pimpl idiom https://en.wikibooks.org/wiki/C%2B%2B_Programming/Idioms#Pointer_To_Implementation_(pImpl)
    class cache_impl;

    std::unique_ptr<cache_impl> m_impl; // pimpl for queue
};

}   // namespace proxy_server_6

#endif // !CACHE_HPP

