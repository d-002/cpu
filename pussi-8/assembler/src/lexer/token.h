#ifndef TOKEN_H
#define TOKEN_H

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

#endif /* ! TOKEN_H */
