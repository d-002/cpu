#ifndef T_IDENTIFIER_H
#define T_IDENTIFIER_H

#include "token.h"

int token_identifier(char *stream, int line, size_t len, struct token *out);

#endif /* ! T_IDENTIFIER_H */
