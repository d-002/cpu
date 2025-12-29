#ifndef NUMSTR_H
#define NUMSTR_H

#include "lexer/token.h"

char *itoa(int n);
int atoi_base(int line, struct token *token);

#endif /* ! NUMSTR_H */
