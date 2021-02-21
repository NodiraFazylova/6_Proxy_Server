#ifndef CACHE_LOCKER_HPP
#define CACHE_LOCKER_HPP

#include <condition_variable>
#include <mutex>
#include <atomic>

namespace proxy_server_6
{
>
class cache_locker
{
public:
    cache_locker( Mutex mtx )
        : m_mtx( mtx )
    {
        m_locked = m_mtx.try_lock();
    }

    ~cache_locker()
    {
        if( m_loced )
        {
            m_mtx.unlock();
        }
    }

    operator bool() const
    {
        return m_locked;
    }

private:

    bool         m_locked;
    std::mutex & m_mtx;
};

} // namespace proxy_server_6

#endif /* CACHE_LOCKER_HPP */
