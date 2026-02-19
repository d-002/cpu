#include "t_data.h"

#include "logger/logger.h"
#include "utils/errors.h"

int token_data(struct string *string, int line, struct token **out)
{
    if (string->length < 2)
    {
        logerror(line, "Syntax error in data location identifier.");
        return LEXING_ERROR;
    }

    enum token_type type;

    char next = string->stream[1];
    if (next == 'r')
        type = REGISTER;
    else if (next == 'm')
        type = MEMORY;
    else if (next == 'p')
        type = PORT;
    else
    {
        logerror(line, "Unrecognized data location type.");
        return LEXING_ERROR;
    }

    size_t i = 2;

    while (i < string->length)
    {
        char c = string->stream[i];
        if (c < '0' || c > '9')
            break;

        i++;
    }

    if (i == 2)
    {
        logerror(line, "Detected empty number in data location identifier.");
        return LEXING_ERROR;
    }

    struct token *token = token_create(type, string->stream, i);
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
