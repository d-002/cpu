#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void log_inner(FILE *stream, const char *prefix, const char *format, va_list args)
{
    fputs(prefix, stream);
    putc(' ', stream);

    vfprintf(stream, format, args);
    putc('\n', stream);
}

void loginfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stdout, "[INFO]", format, args);
    va_end(args);
}

void logerror(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stderr, "[ERROR]", format, args);
    va_end(args);
}
