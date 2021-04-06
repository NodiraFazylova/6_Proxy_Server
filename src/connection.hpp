#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>

#include "answerer.hpp"
#include "logger/logger.hpp"
#include "socket.hpp"


namespace proxy_server_8
{

class connection
{
public:
    connection(
        boost::asio::io_context & io_context,
        proxy_server::answerer & answerer, // @todo
        typename proxy_server::socket::init_type init_parmams,
        typename proxy_server::socket::connect_type connect_params,
        std::function<void( bool )> on_error//,
        //std::function<void()
    )
        : m_io_context( io_context )
        , m_answerer( answerer )
        , m_init_params( init_params )
        , m_connect_params( connect_params )
        , m_socket()
        , m_user_id()
        , m_stopped( false )
        , m_on_error( on_error )
    {
        LOG_DEBUG_IF( m_init_params.verbose, "connection[{0}]::connection()", m_connect_params.port );

        allocate_socket();
    }

private:
    void allocate_socket()
    {
        LOG_DEBUG_IF( "connection[{0}]::allocate_socket() - BEGIN", m_connect_params.port );

        // @todo
    }

public:
    void start()
    {
        LOG_DEBUG_IF( m_init_params.verbose, "connection[{0}]::start() - BEGIN", m_connect_params.port );
        
        m_stopped = false;
        m_socket->start();

        LOG_DEBUG_IF( "connection[{0}]::start() - END", m_connect_params.port );
    }

    void stop()
    {
        LOG_DEBUG_IF( m_init_params.verbose, "connection[{0}]::stop()", m_connect_params.port );

        m_stopped = true;
        stop_impl();
    }

private:
    void stop_impl()
    {
        LOG_DEBUG( "connetion[{0}]::stop_impl() - BEGIN", m_connect_params.port );

        if( m_socket )
        {
            m_socket->stop();
            m_socket.reset();
        }

        LOG_DEBUG( "connection[{0}]::stop_impl() - END", m_connect_params.port );
    }

public:
    
    /**
     * Check whether connection currently active.
     */
    bool is_online() const
    {
        return (m_socket ? m_socket->is_online() : false)
    }

    template<typemame S, typename D>
    size_t send(); ///@todo

private:
    
    /**
     * Called by websocket_connection when any error occured.
     */
    void on_error( bool was_online )
    {
        if( !m_stopped )
        {
            LOG_WARN_IF( m_init_params.verbose, "connection[{0}]::on_error() - first call stop_impl", m_connect_params.port );
            stop_impl();

            LOG_WARN_IF( m_init_params.verbose, "connection[{0}]::on_error() - post upper level's on_error", m_connectio_params.port );
            boost::asio::post( m_io_conntext, std::bind( m_on_error, was_online ) );
        }
    }

private:

    boost::asio::io_context                       & m_io_context;
    proxy_server_6::answerer                      & m_answerer;
    typename proxy_server_6::socket::init_type      m_init_params;
    typename proxy_server_6::socket::connect_type   m_connect_type;

    std::shared_ptr<proxy_server_6::socket>         m_socket;
    
    std::optional<int64_t>                          m_endpoint_id;

    bool                                            m_stopped;

    std::function<void<bool was_online) >           m_on_error;
};

}   // namespace proxy_server_8

#endif // !CONNECTION_HPP
