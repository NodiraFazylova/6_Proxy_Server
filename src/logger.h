#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>

/* 
 * PRINT_ERROR - записывает сообщения об ошибках в stderr
 * PRINT_LOG   - записывает логгируемое сообщение в stdout
 */

#ifdef __linux__ /* если мы на линуксе */
#define RED "\e[0;31m" /* красные */
#define NC "\e[0m" /* сбрасываем цвет на исходный */
#define LOG_ERROR(args...)      fprintf( stderr, RED "[ERROR]" NC " :" ##argc ) /* макрос с функцией для вывода ошибок */
#define LOG_DEBUG(args...)      fprintf( stdout, ##args )
#define LOG_IF(cond, args...)   if((cond)) \ 
                                    fprintf(stdout, ##args )
#elif _WIN32 /* если мы на винде */
#define RED "\x1b[31m" /* красный */
#define NC "\x1b[0m" /* сбрасываем на дефолтный */
#define LOG_ERROR(...)          fprintf(stderr, RED "[ERROR]" NC ": " __VA_ARGS__)
#define LOG_DEBUG(...)          fprintf(stdout, __VA_ARGS__ )
#define LOG_IF(cond, args...)   if((cond)) \
                                    fprintf( stdout, __VA_ARGS__ )

#define LOG_OS_ERROR(OS, message)   OS << message << std::endl
#endif

#endif  /* LOGGER_H */