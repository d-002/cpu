#ifndef NUMSTR_H
#define NUMSTR_H

#include "lexer/token.h"

char *my_itoa(int n);
int atoi_token(struct token *token);
void fill_buf_with_bin(int n, char buf[], int bits);

#endif /* ! NUMSTR_H */
