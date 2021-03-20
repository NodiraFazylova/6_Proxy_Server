#ifndef FILE_QUEUE_HPP
#define FILE_QUEUE_HPP

#include <chrono>
#include <mutex>
#include <set>

namespace proxy_server_6
{

namespace detail
{

class file_queue
{
public:
    struct timed_command
    {
        std::chrono::system_clock::time_point time;
        size_t                                command_hash;
    };

    file_queue() = default;
    ~file_queue() = default;


    file_queue( file_queue & other ) = delete;
    file_queue & operator=( file_queue & other ) = delete;


    file_queue( file_queue && other ) = delete;
    file_queue & operator=( file_queue && other ) = delete;

    void push( timed_command command )
    {
        m_queue.emplace( std::move( command ) );
    }

    std::mutex & get_mutex()
    {
        return m_mtx;
    }

    bool empty() const
    {
        return m_queue.empty();
    }

    timed_command front() const
    {
        return *m_queue.begin();
    }

    void pop()
    {
        m_queue.erase( m_queue.begin() );
    }

    bool contain( const timed_command & command ) const
    {
        return m_queue.count( command );
    }

    void update( const timed_command & command )
    {
        auto pos          = m_queue.find( command );

        auto node         = m_queue.extract( pos );
        node.value().time = command.time;
        
        m_queue.insert( std::move( node ) );
    }

private:

    mutable std::mutex      m_mtx;
    std::set<timed_command> m_queue;
};


inline
bool operator< ( const command_queue::timed_command & lh, const command_queue::timed_command & rh )
{
    return  (lh.time < rh.time) ||
        ((lh.time == rh.time) && (lh.command_hash < rh.command_hash));
}

bool operator== ( const command_queue::timed_command & lh, const command_queue::timed_command & rh )
{
    return (lh.command_hash == rh.command_hash);
}

}   // namespace detail

}   // namespace proxy_server_6

#endif // !FILE_QUEUE_HPP
