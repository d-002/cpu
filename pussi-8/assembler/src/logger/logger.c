#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void log_inner(const char *prefix, int file_line, const char *format,
               va_list args)
{
    fputs(prefix, stderr);

    if (file_line == NO_LINE)
        fputs("assembli: ", stderr);
    else
        fprintf(stderr, "assembli:%d: ", file_line);

    vfprintf(stderr, format, args);
    putc('\n', stderr);
}

void loginfo(int file_line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner("[INFO]  ", file_line, format, args);
    va_end(args);
}

void logwarn(int file_line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner("[WARN]  ", file_line, format, args);
    va_end(args);
}

void logerror(int file_line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner("[ERROR] ", file_line, format, args);
    va_end(args);
}

void log_alloc_error(int file_line)
{
    logerror(file_line, "Failed to allocate memory.");
}

void verbose(struct cli_args *cli_args, int file_line, const char *format, ...)
{
    if (cli_args->verbose == 0)
        return;

    va_list args;
    va_start(args, format);
    log_inner(file_line == NO_LINE ? "" : " ", file_line, format, args);
    va_end(args);
}
