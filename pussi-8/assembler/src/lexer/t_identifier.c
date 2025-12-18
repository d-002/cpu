#include "t_identifier.h"

#include "err.h"
#include "lexer_utils.h"

int token_identifier(struct string string, int line, struct token *out)
{
    size_t i = 0;
    while (i < string.len)
    {
        char c = string.stream[i];
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9')
            && c != '_')
            break;

        i++;
    }

    int res = alloc_token(string.stream, line, i, out);
    if (res)
        return res;

    out->type = IDENTIFIER;

    return SUCCESS;
}
