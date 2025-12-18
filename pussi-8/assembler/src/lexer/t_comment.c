#include "t_comment.h"

#include "err.h"
#include "lexer_utils.h"

int token_comment(struct string string, int line, struct token *out)
{
    out->type = COMMENT;

    int res = alloc_token(string.stream, line, string.len, out);
    if (res)
        return res;

    return SUCCESS;
}
