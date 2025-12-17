#include "t_space.h"

#include "err.h"
#include "lexer_utils.h"

int is_space(char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

int token_space(char *stream, int line, size_t len, struct token *out)
{
    size_t i = 0;
    while (is_space(stream[i]) && i < len)
        i++;

    struct bounds bounds = { 0, i };
    int res = alloc_token(stream, line, bounds, out);
    if (res)
        return res;

    return SUCCESS;
}
