#include "t_comment.h"

#include "logger/logger.h"
#include "utils/errors.h"

int token_comment(struct string *string, int line, struct token **out)
{
    struct token *token = token_create(COMMENT, string->stream, string->length);
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *out = token;
    return SUCCESS;
}
