#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void log_inner(FILE *stream, const char *prefix, int file_line,
               const char *format, va_list args)
{
    fputs(prefix, stream);

    if (file_line == NO_LINE)
        fputs("assembli: ", stream);
    else
        fprintf(stream, "assembli:%d: ", file_line);

    vfprintf(stream, format, args);
    putc('\n', stream);
}

void loginfo(int file_line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stdout, "[INFO ] ", file_line, format, args);
    va_end(args);
}

void logwarn(int file_line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stderr, "[WARN ] ", file_line, format, args);
    va_end(args);
}

void logerror(int file_line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stderr, "[ERROR] ", file_line, format, args);
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
    log_inner(stderr, file_line == NO_LINE ? "" : " ", file_line, format, args);
    va_end(args);
}
