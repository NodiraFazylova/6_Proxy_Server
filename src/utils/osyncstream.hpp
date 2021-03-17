#ifndef OSYNCSTREAM_HPP
#define OSYNCSTREAM_HPP

#include <mutex>
#include <ostream>
#include <sstream>
namespace proxy_server_6
{

class osyncstream : public std::ostringstream
{

public:
	osyncstream(std::ostream & out_stream)
		: m_out_stream(out_stream)
	{}

	~osyncstream()
	{
		std::lock_guard lock( m_mtx );
		m_out_stream << this->str();
	}

private:
	std::ostream	  & m_out_stream;
	static std::mutex	m_mtx;

};

inline
std::mutex osyncstream::m_mtx{};

}	// namespace proxy_server_6

#endif	// OSYNCSTREAM_HPP