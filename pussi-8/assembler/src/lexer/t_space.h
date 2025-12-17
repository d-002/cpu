#ifndef T_SPACE_H
#define T_SPACE_H

#include "token.h"

int is_space(char c);
int token_space(char *stream, int line, size_t len, struct token *out);

#endif /* ! T_SPACE_H */
