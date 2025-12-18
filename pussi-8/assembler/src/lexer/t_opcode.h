#ifndef T_OPCODE_H
#define T_OPCODE_H

#include "mystring.h"
#include "token.h"

int token_opcode(struct string *string, int line, struct token **out);

#endif /* ! T_OPCODE_H */
