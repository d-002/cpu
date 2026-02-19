#ifndef T_OPCODE_H
#define T_OPCODE_H

#include "token.h"
#include "utils/mystring.h"

int token_opcode(struct string *string, int line, struct token **out);

#endif /* ! T_OPCODE_H */
