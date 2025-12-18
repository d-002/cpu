#include "t_simple.h"

#include "err.h"
#include "logger.h"

int token_simple(struct string *string, int line, struct token **out)
{
    char c = string->stream[0];
    enum token_type type;
    if (c == '.')
        type = DOT;
    if (c == ',')
        type = COMMA;
    if (c == ':')
        type = COLON;
    if (c == '=')
        type = EQUAL_SIGN;

    struct token *token = token_create(type, string->stream, 1);
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
