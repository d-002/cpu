#include "t_simple.h"

#include "err.h"
#include "lexer_utils.h"

int token_simple(struct string string, int line, struct token *out)
{
    int res = alloc_token(string.stream, line, 1, out);
    if (res)
        return res;

    char c = string.stream[0];
    if (c == '.')
        out->type = DOT;
    if (c == ',')
        out->type = COMMA;
    if (c == ':')
        out->type = COLON;
    if (c == '=')
        out->type = EQUAL_SIGN;

    return SUCCESS;
}
