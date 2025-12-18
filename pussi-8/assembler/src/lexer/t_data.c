#include "t_data.h"

#include "err.h"
#include "lexer_utils.h"
#include "logger.h"

int token_data(struct string string, int line, struct token *out)
{
    if (string.len < 3)
    {
        logerror(line, "Syntax error in data location identifier.");
        return LEXING_ERROR;
    }

    char next = string.stream[1];
    if (next == 'r')
        out->type = REGISTER;
    else if (next == 'm')
        out->type = MEMORY;
    else if (next == 'p')
        out->type = PORT;
    else
    {
        logerror(line, "Unrecognized data location type.");
        return LEXING_ERROR;
    }

    size_t i = 2;

    while (i < string.len)
    {
        char c = string.stream[i];
        if (c < '0' || c > '9')
            break;

        i++;
    }

    if (i == 2)
    {
        logerror(line, "Detected empty number in data location identifier.");
        return LEXING_ERROR;
    }

    int res = alloc_token(string.stream, line, i, out);
    if (res)
        return res;

    return SUCCESS;
}
