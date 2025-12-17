#include "t_data.h"

#include "err.h"
#include "logger.h"

int token_data(char *stream, int line, size_t len, struct token *out)
{
    if (len == 1)
    {
        logerror(line, "Syntax error in data location identifier.");
        return LEXING_ERROR;
    }

    char next = stream[1];
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

    return 1;
}
