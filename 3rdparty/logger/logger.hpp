#ifndef LOGGER_LOGGER_HPP
#define LOGGER_LOGGER_HPP

#include <string>
#include <spdlog/spdlog.h>

namespace logger
{

void init_null_logger();


void init_console_logger();


void init_rotate_logger( const std::string & path, int max_file_size, int max_files = 3 );


void init_rotate_logger_with_title( const std::string & path, int max_file_size, int max_files = 3 );


void set_rotate_logger_title( const std::string & title );


void init_daily_logger( const std::string & path );


void deinit_logger();


spdlog::logger & logger();

}   // namespace logger


#ifndef LOG_TRACE
#define LOG_TRACE logger::logger().trace
#else
static_assert(false, "LOG_TRACE already defined");
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG logger::logger().debug
#else
static_assert(false, "LOG_DEBUG already defined");
#endif

#ifndef LOG_INFO
#define LOG_INFO logger::logger().info
#else
static_assert(false, "LOG_INFO already defined");
#endif

#ifndef LOG_WARN
#define LOG_WARN logger::logger().warn
#else
static_assert(false, "LOG_WARN already defined");
#endif

#ifndef LOG_ERROR
#define LOG_ERROR logger::logger().error
#else
static_assert(false, "LOG_ERROR already defined");
#endif

#ifdef __linux__
#define LOG_TRACE_IF(cond, args...) if( cond ) LOG_TRACE(args)
#define LOG_DEBUG_IF(cond, args...) if( cond ) LOG_DEBUG(args)
#define LOG_INFO_IF(cond, args...) if( cond ) LOG_INFO(args)
#define LOG_WARN_IF(cond, args...) if( cond ) LOG_WARN(args)
#define LOG_ERROR_IF(cond, args...) if( cond ) LOG_ERROR(args)
#elif _WIN32
#define LOG_TRACE_IF(cond, ...) if( cond ) LOG_TRACE(__VA_ARGS__ )
#define LOG_DEBUG_IF(cond, ...) if( cond ) LOG_DEBUG(__VA_ARGS__ )
#define LOG_INFO_IF(cond, ...) if( cond ) LOG_INFO(__VA_ARGS__ )
#define LOG_WARN_IF(cond, ...) if( cond ) LOG_WARN(__VA_ARGS__ )
#define LOG_ERROR_IF(cond, ...) if( cond ) LOG_ERROR(__VA_ARGS__ )
#endif

#endif  /* LOGGER_LOGGER_HPP */