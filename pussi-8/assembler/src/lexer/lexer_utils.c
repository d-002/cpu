#include "lexer_utils.h"

#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "logger.h"

int alloc_token(char *stream, int line, size_t token_len, struct token *out)
{
    char *data = malloc((token_len + 1) * sizeof(char));

    if (data == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    memcpy(data, stream, token_len);
    data[token_len] = '\0';

    out->data = data;
    out->length = token_len;

    return SUCCESS;
}
