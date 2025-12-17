#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void log_inner(FILE *stream, const char *prefix, int line, const char *format,
               va_list args)
{
    if (line == NO_LINE)
        fputs("assembli: ", stream);
    else
        fprintf(stream, "assembli:%d:", line);

    fputs(prefix, stream);
    vfprintf(stream, format, args);
    putc('\n', stream);
}

void loginfo(int line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stdout, "[INFO]  ", line, format, args);
    va_end(args);
}

void logerror(int line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stderr, "[ERROR] ", line, format, args);
    va_end(args);
}

void log_alloc_error(int line)
{
    logerror(line, "Failed to allocate memory.");
}

void verbose(struct cli_args *cli_args, int line, const char *format, ...)
{
    if (cli_args->verbose == 0)
        return;

    va_list args;
    va_start(args, format);
    log_inner(stderr, "", line, format, args);
    va_end(args);
}
