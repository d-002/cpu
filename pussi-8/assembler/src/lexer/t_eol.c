#include "t_eol.h"

#include <stdlib.h>

#include "err.h"
#include "logger.h"

int token_eol(int line, struct token **out)
{
    struct token *token = malloc(sizeof(struct token));
    if (token == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    token->type = EOL;
    token->data = NULL;
    token->length = 0;

    *out = token;
    return SUCCESS;
}
