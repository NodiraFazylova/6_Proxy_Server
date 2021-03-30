#ifndef FILE_QUEUE_HPP
#define FILE_QUEUE_HPP

#include <chrono>
#include <mutex>
#include <set>

#include "logger/logger.hpp"

namespace proxy_server_6
{

namespace detail
{

class file_queue
{
public:
    struct node_t
    {
        std::chrono::system_clock::time_point time;
        size_t                                filename_hash;
    };

    file_queue() = default;
    ~file_queue() = default;

    file_queue( bool verbose )
        : m_mtx()
        , m_queue()
        , m_verbose( verbose )
    {}


    file_queue( file_queue & other ) = delete;
    file_queue & operator=( file_queue & other ) = delete;


    file_queue( file_queue && other ) = delete;
    file_queue & operator=( file_queue && other ) = delete;

    void push( const node_t & node )
    {
        LOG_TRACE_IF( m_verbose, "file_queue::push()" );
        m_queue.emplace(  node );
    }

    std::mutex & get_mutex()
    {
        LOG_TRACE_IF( m_verbose, "file_queue::get_mutex()" );
        return m_mtx;
    }

    bool empty() const
    {
        LOG_TRACE_IF( m_verbose, "file_queue::empty()" );
        return m_queue.empty();
    }

    node_t front()
    {
        LOG_TRACE_IF( m_verbose, "file_queue::front()" );
        return *m_queue.begin();
    }

    void pop()
    {
        LOG_TRACE_IF( m_verbose, "file_queue::pop()" );
        m_queue.erase( m_queue.begin() );
    }

    bool contain( const node_t & node ) const
    {
        LOG_TRACE_IF( m_verbose, "file_queue::contait()" );
        return m_queue.count( node );
    }

    void update( const node_t & node )
    {
        auto pos          = m_queue.find( node );

        auto node_t         = m_queue.extract( pos );
        node_t.value().time = node.time;
        
        m_queue.insert( std::move( node_t ) );
    }

private:

    mutable std::mutex  m_mtx;
    std::set<node_t>    m_queue;

    bool                m_verbose = false;
};


inline
bool operator< ( const file_queue::node_t & lh, const file_queue::node_t & rh )
{
    return  (lh.time < rh.time) ||
        ((lh.time == rh.time) && (lh.filename_hash < rh.filename_hash));
}

inline
bool operator== ( const file_queue::node_t & lh, const file_queue::node_t & rh )
{
    return (lh.filename_hash == rh.filename_hash);
}

}   // namespace detail

}   // namespace proxy_server_6

#endif // !FILE_QUEUE_HPP
