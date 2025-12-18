#ifndef T_SPACE_H
#define T_SPACE_H

#include "mystring.h"
#include "token.h"

int is_space(char c);
int token_space(struct string string, int line, struct token *out);

#endif /* ! T_SPACE_H */
