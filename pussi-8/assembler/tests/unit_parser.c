#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "err.h"
#include "parser/parser.h"

TestSuite(Parser);

struct current_string
{
    char *str;
    size_t len;
    size_t pointer;
};

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

    size_t len = i - current_string.pointer;
    if (*buf_ptr == NULL || *n < len)
        *buf_ptr = realloc(*buf_ptr, len + 1);

    (*buf_ptr)[len] = '\0';
    memcpy(*buf_ptr, current_string.str + current_string.pointer, len);
    current_string.pointer += len;

    return len;
}

Test(Parser, Empty)
{
    init_current_string("hello world\nabc");
    free_current_string();
}
