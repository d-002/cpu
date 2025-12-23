#include "unit_parser_utils.h"

#include <stdlib.h>
#include <string.h>

struct current_string current_string = {
    .str = NULL,
    .len = 0,
    .pointer = 0,
};

void init_current_string(char *str)
{
    size_t len = strlen(str);
    current_string.str = malloc(len + 1);
    current_string.len = len;
    current_string.pointer = 0;

    memcpy(current_string.str, str, len + 1);
}

void free_current_string(void)
{
    free(current_string.str);
    current_string.str = NULL;
    current_string.len = 0;
    current_string.pointer = 0;
}

ssize_t get_fake_line(char **buf_ptr, size_t *n,
                      __attribute__((unused)) FILE *stream)
{
    size_t i = current_string.pointer;
    while (i < current_string.len && current_string.str[i] != '\n')
        i++;

    if (i == current_string.len)
        i--;

    size_t len = i - current_string.pointer + 1;
    if (*buf_ptr == NULL || *n < len)
        *buf_ptr = realloc(*buf_ptr, len + 1);

    (*buf_ptr)[len] = '\0';
    memcpy(*buf_ptr, current_string.str + current_string.pointer, len);
    current_string.pointer += len;

    return len;
}
