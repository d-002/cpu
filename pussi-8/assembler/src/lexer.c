#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>

#include "err.h"
#include "logger.h"

int is_space(char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

int next_token(char *stream, int line, size_t len, struct token *out)
{
    out->data = NULL;

    if (!len)
    {
        logerror(line, "Failed to read token.");
        return LEXING_ERROR;
    }

    char c = stream[0];

    if (is_space(c))
        out->type = SPACE;
    else if (isalpha(c))
        out->type = IDENTIFIER;
    else if (c == '0')
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
    }
    else if (c == '%')
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
    }
    else if (c == ';')
        out->type = COMMENT;
    else
        out->type = OPCODE;

    return 1;
    out->data = NULL;
    out->length = 1;
    return SUCCESS;
}
