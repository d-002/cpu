#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>

#include "err.h"
#include "logger.h"
#include "t_comment.h"
#include "t_data.h"
#include "t_identifier.h"
#include "t_number.h"
#include "t_opcode.h"
#include "t_space.h"

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
        return token_space(stream, line, len, out);
    if (isalpha(c))
        return token_identifier(stream, line, len, out);
    if (c == '0')
        return token_number(stream, line, len, out);
    if (c == '%')
        return token_data(stream, line, len, out);
    if (c == ';')
        return token_comment(stream, line, len, out);
    return token_opcode(stream, line, len, out);
}
