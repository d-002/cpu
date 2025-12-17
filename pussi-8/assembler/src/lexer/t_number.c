#include "t_number.h"

#include "err.h"
#include "logger.h"

int token_number(char *stream, int line, size_t len, struct token *out)
{
    if (len == 1)
    {
        logerror(line, "Syntax error in number.");
        return LEXING_ERROR;
    }

    char next = stream[1];
    if (next == 'b')
        out->type = NUMBER_BIN;
    else if (next == 'h')
        out->type = NUMBER_HEX;
    else
        out->type = NUMBER_DEC;

    return 1;
}
