#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stddef.h>

#include "token.h"

struct bounds
{
    size_t start; // included
    size_t end; // excluded
};

int alloc_token(char *stream, int line, struct bounds bounds,
                struct token *out);

#endif /* ! LEXER_UTILS_H */
