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
#include "t_simple.h"
#include "t_space.h"

int next_token(struct string string, int line, int expecting_opcode,
               struct token **out)
{
    if (!string.len)
    {
        logerror(line, "Failed to read next token.");
        return LEXING_ERROR;
    }

    char c = string.stream[0];

    if (is_space(c))
        return token_space(string, line, out);
    if (isalpha(c))
        return expecting_opcode ? token_opcode(string, line, out)
                                : token_identifier(string, line, out);
    if (isdigit(c))
        return token_number(string, line, out);
    if (c == '%')
        return token_data(string, line, out);
    if (c == ';')
        return token_comment(string, line, out);
    if (c == '.' || c == ',' || c == ':' || c == '=')
        return token_simple(string, line, out);

    logerror(line, "Failed to identify token type.");
    return LEXING_ERROR;
}
