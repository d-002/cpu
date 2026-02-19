#ifndef LOGGER_H
#define LOGGER_H

#include "cli/cli.h"

#define NO_LINE -1

void loginfo(int line, const char *format, ...);
void logerror(int line, const char *format, ...);
void log_alloc_error(int line);
void verbose(struct cli_args *cli_args, int line, const char *format, ...);

#endif /* ! LOGGER_H */
