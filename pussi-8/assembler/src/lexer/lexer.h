#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "token.h"
#include "utils/mystring.h"

int next_token(struct string *string, int line, bool expecting_opcode,
               struct token **out);

#endif /* ! LEXER_H */
