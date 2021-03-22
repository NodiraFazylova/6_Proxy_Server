#ifndef RUN_IO_CONTEXT_HPP
#define RUN_IO_CONTEXT_HPP

#include <iostream>
#include <stdexcept>

#include <boost/asio.hpp>

#include "logger/logger.hpp"

namespace proxy_server_6
{

template <typename T>
class io_run_context_t
{
public:
	io_run_context_t( T & io_context )
		: m_io_context(io_context)
	{}

	void operator()() noexcept
	{
		while( true )
		{
			try
			{
				m_io_context.run(); // block here
				break;
			}
			catch( std::exception & e )
			{
				LOG_ERROR( "run_io_context_t(): EXCEPTION: {0}", e.what() );
			}
			catch( ... )
			{
				LOG_ERROR( "run_io_context_t(): UNKNOWN EXCEPTION" );
			}
		}
	}

private:
	T & m_io_context;
};


template <typename T>
io_run_context_t<T> run_io_context( T & io_context )
{
	return io_run_context_t( io_context );
}

template <typename T>
class io_run_context_once_t
{
public:
	io_run_context_once_t( T & io_context )
		: m_io_context( io_context )
	{}

	void operator()() noexcept
	{
		try
		{
			m_io_context.run(); // block here
		}
		catch( std::exception & e )
		{
			LOG_ERROR( "run_io_context_t(): EXCEPTION: {0}", e.what() );
		}
		catch( ... )
		{
			LOG_ERROR( "run_io_context_t(): UNKNOWN EXCEPTION" );
		}
	}

private:
	T & m_io_context;
};


template <typename T>
io_run_context_once_t<T> io_run_context_once( T & io_context )
{
	return io_run_context_once_t( io_context );
}

}	// proxy_server_6

#endif	/* RUN_IO_CONTEXT_HPP */