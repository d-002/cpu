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

struct token *token_create(enum token_type type, char *stream, size_t len);
char *type2name(enum token_type type);
void token_destroy(struct token *token);

#endif /* ! TOKEN_H */
