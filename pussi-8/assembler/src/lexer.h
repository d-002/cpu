#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

enum token_type
{
    UNKNOWN = 0,

    SPACE,
    IDENTIFIER,
    NUMBER,
    REGISTER,
    MEMORY,
    PORT,
    TEXT,
    OPCODE,

    COLON,
    EQUAL_SIGN,
    DOT,
    COMMA
};

struct token
{
    enum token_type type;
    char *data;
};

int next_token(int fd, struct token *out);

#endif /* ! LEXER_H */
