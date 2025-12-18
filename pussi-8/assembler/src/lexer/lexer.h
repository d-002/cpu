#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "mystring.h"
#include "token.h"

int next_token(struct string string, int line, int expecting_opcode,
               struct token *out);

#endif /* ! LEXER_H */
