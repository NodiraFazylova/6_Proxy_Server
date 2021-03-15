#ifndef BUCKET_HPP
#define BUCKET_HPP

#include <map>
#include <mutex>
#include <string>

namespace proxy_server_6
{
namespace detail
{

class bucket
{
public:
    bucket() = default;
    ~bucket() = default;


    bucket( bucket & other ) = delete;
    bucket & operator=( bucket & other ) = delete;


    bucket( cache && other ) = delete;
    bucket & operator=( bucket && other ) = delete;


    void insert( const size_t command_hash, const std::string & file )
    {
        m_buckets.emplace(command_hash, file);
    }


    void erase( const size_t command_hash )
    {
        m_buckets.erase( command_hash );
    }


    std::string get_file( const size_t command_hash )
    {
        if( m_buckets.count( command_hash ) != 0 )
        {
            return m_buckets[command_hash];
        }

        return {};
    }


    void get_all_files(std::vector<std::string> & files)
    {
        for( auto & [hash, file] : m_buckets )
        {
            files.emplace_back( file );
        }
    }

    std::mutex & get_mutex()
    {
        return m_mtx;
    }

private:
    std::mutex                      m_mtx;
    std::map<size_t, std::string>   m_buckets;
};

}   // namespace detail

}   // namespace proxy_server_6

#endif // !BUCKET_HPP
