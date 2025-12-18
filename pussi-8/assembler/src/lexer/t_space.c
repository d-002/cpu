#include "t_space.h"

#include "err.h"
#include "logger.h"

int is_space(char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

int token_space(struct string string, int line, struct token **out)
{
    size_t i = 0;
    while (is_space(string.stream[i]) && i < string.len)
        i++;

    struct token *token = token_create(SPACE, string.stream, i);

    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
