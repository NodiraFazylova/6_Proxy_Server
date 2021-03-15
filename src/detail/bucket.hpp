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
        m_files.insert( command_hash, file );
    }

    void erase( const size_t command_hash )
    {
        m_files.erase( command_hash );
    }

    std::string get_file( const size_t command_hash )
    {
        if( m_files.count( command_hash ) != 0 )
        {
            return = m_files[command_hash];
        }

        return {};
    }

    std::mutex & get_mutex()
    {
        return m_mtx;
    }

private:
    std::mutex                      m_mtx;
    std::map<size_t, std::string>   m_files;
};

}   // namespace detail

}   // namespace proxy_server_6

#endif // !BUCKET_HPP
