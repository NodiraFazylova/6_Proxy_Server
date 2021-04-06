#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <functional>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "logger/logger.hpp"

namespace proxy_server_6
{

class socket : public std::enable_shared_from_this<socket>
{
public:

    using error_code = boost::beast::error_code;


    /**
     * Specifies socket initialization parameters.
     */
    struct init_type
    {
        boost::asio::io_context & io_context;
        bool                      verbose;
    };


    /** 
     * Specifies socket connection parameters.
     */
    struct connect_type
    {
        std::string host;
        std::string port;

        size_t      conntect_inactivity_timeout;    /**< timeout used while connecting */
        size_t      online_inactivity_timeout;      /**< timeout used when connection established successfully */
    };

public:


    /**
     * Initialize socket.
     */
    socket( socket::init_type init_params )
        : m_io_context( init_params.io_context )
        , m_resolver( m_io_context )
        , m_socket( m_io_context )
        , m_host()
        , m_send_buffer()
        , m_receive_buffer()
        , m_verbose(init_params.verbose)
    {
        LOG_TRACE_IF( m_verbose, "socket::socket()" );
    }


    /**
     * Close internal socket and cansel any asynchronous operations.
     */
    template<typename F>
    void async_close( F && on_close )
    {
        LOG_TRACE_IF( m_verbose, "socket::async_close() - BEGIN" );

        m_resolver.cancel();
        m_socket.async_close( boost::beast::websocket::close_code::normal, std::forward<F>( on_close ) );

        LOG_TRACE_IF( "socket::async_close() - END" );
    }

public:

    /**
     * Asynchronous connect to the specified host.
     * 
     * @note Because socket connecting consist from resolve, connect and handshake operation
     * internal handlers will be used to handle results from all of them.
     */
    template<typename F>
    void async_connect( socket::connect_type connect_params, F && on_connect )
    {
        LOG_TRACE_IF( m_verbose, "socket::async_connect() - start async_resolve, host={0} port={1}", connect_params.host, connect_params.port );

        std::function<void( socket::error_code error_code )> bind_on_connect( std::forward<F>( on_connect ) );
        m_host = connect_params.host;

        m_resolver.async_resolve(
            connect_params.host,
            connect_params.port,
            std::bind(
                &socket::handle_resolve,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2,
                bind_on_connect
            )
        );
    }

private:

    /**
     * Internal hadler for async_resolve.
     */
    void handle_resolve( socket::error_code error_code, boost::asio::ip::tcp::resolver::results_type results, std::function<void( socket::error_code error_code )>  bind_on_connect )
    {
        if( error_code )
        {
            LOG_WARN_IF( m_verbose, "socket::handle_resolve() - error_code={0}", error_code.message() );

            return post_handler( std::move( bind_on_connect ), error_code );
        }

        LOG_TRACE_IF( m_verbose, "socket::handle_resolve() - start async_connect" );

        boost::asio::async_connect(
            m_socket.next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &socket::handle_connect,
                shared_from_this(),
                std::placeholders::_1,
                bind_on_connect
            )
        );
    }


    /**
     * Internal handler for async_connect.
     */
    void handle_connect( socket::error_code error_code, std::function<void( socket::error_code error_code )> bind_on_connect )
    {
        if( error_code )
        {
            LOG_WARN_IF( m_verbose, "socket::handle_connect() - error_code={0}", error_code.message() );

            return post_handler( std::move( bind_on_connect ), error_code );
        }

        LOG_TRACE_IF( m_verbose, "socket::handle_connect() - start async_handshake" );

        m_socket.async_handshake(
            m_host,
            "/",
            std::bind(
                &socket::handle_handshake,
                shared_from_this(),
                std::placeholders::_1,
                bind_on_connect
            )
        );
    }


    ///**
    // * Internal handler for ssl asynct_handshake.
    // */
    //void handle_ssl_handshake( socket::error_code error_code, std::function<void( socket::error_code error_code )> bind_on_connect )
    //{
    //    if( error_code )
    //    {
    //        LOG_WARN_IF( m_verbose, "socket::handle_ssl_handshake() - error_code={0}", error_code.message() );

    //        return  post_handler( std::move( bind_on_connect ), error_code );
    //    }

    //    LOG_TRACE_IF( "socket::handle_ssl_handshake() - start async_handshake" );

    //    m_socket.async_handshake(
    //        m_host,
    //        "/",
    //        std::bind(
    //            &socket::handle_handshake,
    //            shared_from_this(),
    //            std::placeholders::_1,
    //            bind_on_connect
    //        )
    //    );
    //}


    /**
     * Internal handler for websocket async_handshake.
     */
    void handle_handshake( socket::error_code error_code, std::function<void( socket::error_code error_code )> bind_on_connect )
    {
        if( error_code )
        {
            LOG_WARN_IF( m_verbose, "socket::handle_handshake() - error_code={0}", error_code.message() );
        }
        else
        {
            LOG_WARN_IF( m_verbose, "socket::handle_handshake() - success" );
        }

        post_handler( std::move( bind_on_connect ), error_code );
    }

public:

    /**
     * Start asynchronous write operation.
     */
    template<typename F>
    void async_write( std::string buffer, F && on_write )
    {
        LOG_TRACE_IF( m_verbose, "socket::async_write()" );

        m_send_buffer = std::move( buffer );
        std::function<void( socket::error_code error_code )> bind_on_write( std::forward<F>( on_write ) );

        m_socket.async_write(
            boost::asio::buffer( m_send_buffer ),
            std::bind(
                &socket::handle_write,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2,
                bind_on_write
            )
        );
    }

private:

    /**
     * Internal handler for async_write.
     */
    void handle_write( socket::error_code error_code, std::size_t bytes_transferred, std::function<void( socket::error_code error_code )> bind_on_write )
    {
        if( error_code )
        {
            LOG_WARN_IF( m_verbose, "socket::handle_write() - error_code={0}", error_code.message() );
        }
        else
        {
            LOG_TRACE_IF( m_verbose, "socket::handle_write() - success" );
        }

        boost::ignore_unused( bytes_transferred );
        post_handler( std::move( bind_on_write ), error_code );
    }

public:

    /**
     * Start asynchronous read opetation.
     */
    template<typename F>
    void sync_read( F && on_read )
    {
        LOG_TRACE_IF( m_verbose, "socket::async_read()" );

        std::function<void( socket::error_code error_code, std::string buffer )> bind_on_read( std::forward<F>( on_read ) );

        m_socket.async_read(
            m_receive_buffer,
            std::bind(
                &socket::handle_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2,
                bind_on_read
            )
        );
    }

private:

    /**
     * Internal handler for async_write.
     */
    void hadle_read( socket::error_code error_code, std::size_t bytes_transferred, std::function<void( socket::error_code error_code, std::string buffer )> bind_on_read )
    {
        if( error_code )
        {
            LOG_WARN_IF( m_verbose, "socket::handle_read() - error_code={0}", error_code.message() );
        }
        else
        {
            LOG_TRACE_IF( m_verbose, "socket::handle_read() - success" );
        }

        boost::ignore_unused( bytes_transferred );
        post_handler( std::move( bind_on_read ), error_code, boost::beast::buffers_to_string( m_receive_buffer.data() ) );
        m_receive_buffer.consume( m_receive_buffer.size() );
    }

private:

    /**
     * Post specified handler with provided params - single param version for async_connect, async_write.
     */
    void post_handler( std::function<void( socket::error_code error_code )> handler, socket::error_code error_code )
    {
        boost::asio::post(
            m_io_context,
            std::bind(
                handler,
                error_code
            )
        );
    }


    /**
     * Post specified handler with provided params - two params version for async_read
     */
    void post_handler( std::function<void( socket::error_code error_code, std::string buffer )> handler, socket::error_code error_code, std::string buffer )
    {
        boost::asio::post(
            m_io_context,
            std::bind(
                handler,
                error_code,
                buffer
            )
        );
    }

private:

    using beast_websocket_t = boost::beast::websocket::stream< boost::asio::ip::tcp::socket >;

    boost::asio::io_context       & m_io_context;
    boost::asio::ip::tcp::resolver  m_resolver;
    
    beast_websocket_t               m_socket;

    std::string                     m_host;
    std::string                     m_send_buffer;
    boost::beast::multi_buffer      m_receive_buffer;

    bool                            m_verbose;
};

} // namespace proxy_server_6

#endif  // SOCKET_HPP