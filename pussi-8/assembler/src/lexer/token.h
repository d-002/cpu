#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stddef.h>

enum token_type
{
    EOL = 0,
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

    NUMBER_PSEUDOTYPE, // to use internally outside of lexing
};

struct token
{
    enum token_type type;
    char *data;
    size_t length;
};

struct token *token_create(enum token_type type, char *stream, size_t len);
char *type2name(enum token_type type);
void token_destroy(struct token *token, bool empty_data);

#endif /* ! TOKEN_H */
