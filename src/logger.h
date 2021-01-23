#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>

/* 
 * PRINT_ERROR - ���������� ��������� �� ������� � stderr
 * PRINT_LOG   - ���������� ����������� ��������� � stdout
 */

#ifdef __linux__ /* ���� �� �� ������� */
#define RED "\e[0;31m" /* ������� */
#define NC "\e[0m" /* ���������� ���� �� �������� */
#define LOG_ERROR(args...)      fprintf( stderr, RED "[ERROR]" NC " :" ##argc ) /* ������ � �������� ��� ������ ������ */
#define LOG_DEBUG(args...)      fprintf( stdout, ##args )
#define LOG_IF(cond, args...)   if((cond)) \ 
                                    fprintf(stdout, ##args )
#elif _WIN32 /* ���� �� �� ����� */
#define RED "\x1b[31m" /* ������� */
#define NC "\x1b[0m" /* ���������� �� ��������� */
#define LOG_ERROR(...)          fprintf(stderr, RED "[ERROR]" NC ": " __VA_ARGS__)
#define LOG_DEBUG(...)          fprintf(stdout, __VA_ARGS__ )
#define LOG_IF(cond, args...)   if((cond)) \
                                    fprintf( stdout, __VA_ARGS__ )

#define LOG_OS_ERROR(OS, message)   OS << message << std::endl
#endif

#endif  /* LOGGER_H */