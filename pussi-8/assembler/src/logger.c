#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void log_inner(FILE *stream, const char *prefix, const char *format, va_list args)
{
    fputs(prefix, stream);
    vfprintf(stream, format, args);
    putc('\n', stream);
}

void loginfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stdout, "[INFO]  ", format, args);
    va_end(args);
}

void logerror(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stderr, "[ERROR] ", format, args);
    va_end(args);
}

void log_alloc_error() {
    logerror("Failed to allocate memory.");
}

void verbose(struct cli_args *cli_args, const char *format, ...) {
    if (cli_args->verbose == 0)
        return;

    va_list args;
    va_start(args, format);
    log_inner(stderr, "", format, args);
    va_end(args);
}
