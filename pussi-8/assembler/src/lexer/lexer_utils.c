#include "lexer_utils.h"

#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "logger.h"

int alloc_token(char *stream, int line, size_t len, struct token *out)
{
    char *data = malloc((len + 1) * sizeof(char));

    if (data == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    memcpy(data, stream, len);
    data[len] = '\0';

    out->data = data;
    out->length = len;

    return SUCCESS;
}
