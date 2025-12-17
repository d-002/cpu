#include "lexer_utils.h"

#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "logger.h"

int alloc_token(char *stream, int line, struct bounds bounds, struct token *out)
{
    size_t len = bounds.end - bounds.start;
    char *data = malloc((len + 1) * sizeof(char));

    if (data == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    memcpy(data, stream + bounds.start, len);
    data[len] = '\0';

    out->data = data;
    out->length = len;

    return SUCCESS;
}
