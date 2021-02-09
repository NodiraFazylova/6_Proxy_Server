#ifndef LOGGER_H
#define LOGGER_H

/* 
 * LOG_ERROR - ���������� ��������� �� ������� � file
 * LOG_DEBUG - ���������� ����������� ��������� � ostream
 * LOG_IF    - ���������� ����������� ��������� ���� ��������� �������
 */

#ifdef LOG_ENABLE

  #ifdef __linux__ /* ���� �� �� ������� */
    #define RED "\e[0;31m" /* ������� */
    #define NC "\e[0m" /* ���������� ���� �� �������� */

    #define LOG_ERROR_C(file, args...)       fprintf( file, RED "[ERROR]" NC " :" ##argc ) /* ������ � �������� ��� ������ ������ */
    #define LOG_DEBUG_C(file, args...)       fprintf( file, "[DEBUG]: " ##args )
    #define LOG_IF_C(cond, file,  args...)   if((cond)) \ 
                                                 LOG_DEBUG( file, args... )
  
  #elif _WIN32 /* ���� �� �� ����� (power-shell) */
    #define RED "\x1b[31m" /* ������� */
    #define NC "\x1b[0m" /* ���������� �� ��������� */
  
    #define LOG_ERROR_C(file, ...)        fprintf(file, RED "[ERROR]" NC ": " __VA_ARGS__)
    #define LOG_DEBUG_C(file, ...)        fprintf(file, "[DEBUG]: " __VA_ARGS__ )
    #define LOG_IF_C(cond, file,  ...)    if((cond)) \
                                              LOG_DEBUG_C(file, ...)
    #endif /* OS */

  #ifdef __cplusplus
    #define LOG_ERROR(OS, message)        OS << RED "[ERROR]" NC ": " << message << std::endl
    #define LOG_DEBUG(OS, message)        OS << "[DEBUG]: " << message << std::endl;
    #define LOG_IF(cond, OS, message)     if((cond)) \
                                              LOG_DEBUG(OS, message)
  #else
    #define LOG_ERROR   LOG_ERROR_C
    #define LOG_DEBUG   LOG_DEBUG_C
    #define LOG_IF      LOG_IF_C
  #endif /*  __cplusplus */

#else
  #define LOG_ERROR
  #define LOG_DEBUG
  #define LOG_IF

#endif /* LOG_ENABLE */

#endif  /* LOGGER_H */