#include "t_space.h"

#include "err.h"
#include "lexer_utils.h"

int is_space(char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

int token_space(struct string string, int line, struct token *out)
{
    size_t i = 0;
    while (is_space(string.stream[i]) && i < string.len)
        i++;

    int res = alloc_token(string.stream, line, i, out);
    if (res)
        return res;

    out->type = SPACE;

    return SUCCESS;
}
