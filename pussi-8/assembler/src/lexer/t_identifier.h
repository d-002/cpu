#ifndef T_IDENTIFIER_H
#define T_IDENTIFIER_H

#include "token.h"
#include "utils/mystring.h"

int token_identifier(struct string *string, int line, struct token **out);

#endif /* ! T_IDENTIFIER_H */
