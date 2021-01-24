#ifndef SERVER_HPP
#define SERVER_HPP

#include <optional>
#include <stdint.h>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/thread.hpp>

namespace proxy_server_6
{

class server
{
public:
    struct config_t
    {
        std::string host;
        std::string port;
        std::string dir_path{"."};
        int64_t     maxdata_size = 1024;
        int64_t     workers_count = 8;
        bool        verbose = false;
    };

public:
    server( config_t config );

    ~server();

private:
    server(const proxy_server_6::server & ) = delete;

    server & operator= ( const server & ) = delete;

public:
    void start();

    void stop();

private:
    using asio_work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    boost::thread_group                 m_threads;
    boost::asio::io_context             m_io_context;
    std::optional<asio_work_guard_t>    m_io_context_work;

    std::string                         m_host;
    std::string                         m_port;
    std::string                         m_dir_path;
    int64_t                             m_maxdata_size;
    int64_t                             m_workers_count;
    bool                                m_verbose;
};

bool is_valid_config( proxy_server_6::server::config_t & config );

}   // namespace proxy_server_6

#endif /* SERVER_HPP */
