#ifndef COMMAND_QUEUE_HPP
#define COMMAND_QUEUE_HPP

#include <chrono>
#include <mutex>
#include <set>

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

    void insert( timed_command command )
    {
        auto pos = std::begin( m_queue );
        for( auto end_pos = std::end( m_queue ); pos != end_pos; pos = std::next( pos ) )
        {
            if( pos->command_hash == command.command_hash )
            {
                break;
            }
        }

        if( pos != std::end( m_queue ) )
        {
            auto node = m_queue.extract( pos );
            node.value().time = command.time;
            m_queue.insert( std::move( node ) );

            return;
        }

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

    size_t get_front_command() const
    {
        return m_queue.begin()->command_hash;
    }

    void delete_front_command()
    {
        m_queue.erase( m_queue.begin() );
    }

private:

    mutable std::mutex              m_mtx;
    std::set<timed_command> m_queue;
};


inline
bool operator< ( const command_queue::timed_command & lh, const command_queue::timed_command & rh )
{
    return  (lh.time < rh.time) ||
        ((lh.time == rh.time) && (lh.command_hash < rh.command_hash));
}

}   // namespace detail

}   // namespace proxy_server_6

#endif // !COMMAND_QUEUE_HPP
