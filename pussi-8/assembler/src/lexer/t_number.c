#include "t_number.h"

#include "err.h"
#include "lexer_utils.h"
#include "logger.h"

int token_number(struct string string, int line, struct token *out)
{
    out->type = NUMBER_DEC;
    size_t i = 0;

    if (string.stream[0] == '0' && string.len >= 2)
    {
        char next = string.stream[1];
        if (next == 'b')
        {
            out->type = NUMBER_BIN;
            i = 2;
        }
        else if (next == 'x')
        {
            out->type = NUMBER_HEX;
            i = 2;
        }

        if (string.len <= i)
        {
            logerror(line, "Syntax error in number");
            return LEXING_ERROR;
        }
    }

    while (i < string.len)
    {
        char c = string.stream[i];
        if (out->type == NUMBER_BIN && (c < '0' || c > '1'))
            break;
        int number = '0' <= c && c <= '9';
        if (out->type == NUMBER_HEX && (c < 'a' || c > 'f') && !number)
            break;
        if (out->type == NUMBER_DEC && !number)
            break;

        i++;
    }

    int res = alloc_token(string.stream, line, i, out);
    if (res)
        return res;

    return SUCCESS;
}
