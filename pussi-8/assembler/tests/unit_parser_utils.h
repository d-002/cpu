#ifndef UNIT_PARSER_UTILS_H
#define UNIT_PARSER_UTILS_H

#include <stdio.h>
#include <sys/types.h>

struct current_string
{
    char *str;
    size_t len;
    size_t pointer;
};

void init_current_string(char *str);
void free_current_string(void);
ssize_t get_fake_line(char **buf_ptr, size_t *n,
                      __attribute__((unused)) FILE *stream);

#endif /* ! UNIT_PARSER_UTILS_H */
