#include "t_space.h"

#include "err.h"
#include "logger.h"

int is_space(char c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

int token_space(char *stream, int line, size_t len, struct token *out)
{
    stream++;
    line++;
    len++;
    out++;

    return 1;
}
