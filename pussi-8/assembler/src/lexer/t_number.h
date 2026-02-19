#ifndef T_NUMBER_H
#define T_NUMBER_H

#include "token.h"
#include "utils/mystring.h"

int token_number(struct string *string, int line, struct token **out);

#endif /* ! T_NUMBER_H */
