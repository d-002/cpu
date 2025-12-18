#include "t_opcode.h"

#include <ctype.h>

#include "err.h"
#include "lexer_utils.h"

int token_opcode(struct string string, int line, struct token *out)
{
    size_t i = 0;

    while (i < string.len)
    {
        if (!isalpha(string.stream[i]))
            break;

        i++;
    }

    int res = alloc_token(string.stream, line, i, out);
    if (res)
        return res;

    return SUCCESS;
}
