#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "token.h"

int next_token(char *stream, int line, size_t len, struct token *out);

#endif /* ! LEXER_H */
