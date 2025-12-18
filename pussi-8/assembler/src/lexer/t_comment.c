#include "t_comment.h"

#include "err.h"
#include "logger.h"

int token_comment(struct string string, int line, struct token **out)
{
    struct token *token = token_create(COMMENT, string.stream, string.len);
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
