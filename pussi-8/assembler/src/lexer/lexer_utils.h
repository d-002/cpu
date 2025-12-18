#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stddef.h>

#include "token.h"

int alloc_token(char *stream, int line, size_t token_len, struct token *out);

#endif /* ! LEXER_UTILS_H */
