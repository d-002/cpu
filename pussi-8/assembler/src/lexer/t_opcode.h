#ifndef T_OPCODE_H
#define T_OPCODE_H

#include "token.h"

int token_opcode(char *stream, int line, size_t len, struct token *out);

#endif /* ! T_OPCODE_H */
