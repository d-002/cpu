#include "t_opcode.h"

#include <ctype.h>

#include "err.h"
#include "logger.h"

int token_opcode(struct string *string, int line, struct token **out)
{
    size_t i = 0;

    while (i < string->len)
    {
        if (!isalpha(string->stream[i]))
            break;

        i++;
    }

    struct token *token = token_create(OPCODE, string->stream, i);
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
