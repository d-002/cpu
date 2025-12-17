#ifndef LOGGER_H
#define LOGGER_H

void loginfo(const char *format, ...);
void logerror(const char *format, ...);
void log_alloc_error();

#endif /* ! LOGGER_H */
