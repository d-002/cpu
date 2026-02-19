#include "t_identifier.h"

#include "logger/logger.h"
#include "utils/errors.h"

int token_identifier(struct string *string, int line, struct token **out)
{
    size_t i = 0;
    while (i < string->length)
    {
        char c = string->stream[i];
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9')
            && c != '_')
            break;

        i++;
    }

    struct token *token = token_create(IDENTIFIER, string->stream, i);
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
