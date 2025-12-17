#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

enum token_type
{
    SPACE,
    IDENTIFIER,
    NUMBER_BIN,
    NUMBER_HEX,
    NUMBER_DEC,
    REGISTER,
    MEMORY,
    PORT,
    COMMENT,
    OPCODE,

    DOT,
    COMMA,
    COLON,
    EQUAL_SIGN,
};

struct token
{
    enum token_type type;
    char *data;
    size_t length;
};

int next_token(char *stream, int line, size_t len, struct token *out);

#endif /* ! LEXER_H */
