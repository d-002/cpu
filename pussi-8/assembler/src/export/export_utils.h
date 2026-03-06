#ifndef EXPORT_UTILS_H
#define EXPORT_UTILS_H

#include <stdbool.h>
#include <stddef.h>

size_t get_extension_dot_index(char *path, bool *found);
char *change_extension(char *path, char *ext);

#endif /* ! EXPORT_UTILS_H */
