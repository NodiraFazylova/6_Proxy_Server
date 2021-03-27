#ifndef FILE_MAP_HPP
#define FILE_MAP_HPP

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace proxy_server_6
{
namespace detail
{

class file_map
{
public:
    file_map() = default;
    ~file_map() = default;


    file_map( file_map & other ) = delete;
    file_map & operator=( file_map & other ) = delete;


    file_map( file_map && other ) = delete;
    file_map & operator=( file_map && other ) = delete;


    void insert( const size_t filename_hash, const std::string & file )
    {
        m_files.emplace( filename_hash, file);
    }


    void erase( const size_t command_hash )
    {
        m_files.erase( command_hash );
    }


    std::string get_file( const size_t command_hash ) const
    {
        if( m_files.count( command_hash ) != 0 )
        {
            return m_files.at(command_hash);
        }

        return {};
    }


    void get_all_files(std::vector<std::string> & files) const
    {
        for( auto & [hash, file] : m_files )
        {
            files.emplace_back( file );
        }
    }

    // for the sake of simplicity, let's make a design error and label a function that is not "const" 
    std::mutex & get_mutex() const
    {
        return m_mtx;
    }

private:
    mutable std::mutex              m_mtx;
    std::map<size_t, std::string>   m_files;
};

}   // namespace detail

}   // namespace proxy_server_6

#endif // !FILE_MAP_HPP
