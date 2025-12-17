#ifndef T_NUMBER_H
#define T_NUMBER_H

#include "token.h"

int token_number(char *stream, int line, size_t len, struct token *out);

#endif /* ! T_NUMBER_H */
