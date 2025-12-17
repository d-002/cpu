#include "t_comment.h"

#include "err.h"
#include "logger.h"

int token_comment(char *stream, int line, size_t len, struct token *out)
{
    stream++;
    line++;
    len++;
    out++;

    return 1;
}
