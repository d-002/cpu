#ifndef LOGGER_H
#define LOGGER_H

#include "cli.h"

void loginfo(const char *format, ...);
void logerror(const char *format, ...);
void log_alloc_error();
void verbose(struct cli_args *cli_args, const char *format, ...);

#endif /* ! LOGGER_H */
