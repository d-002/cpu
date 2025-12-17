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

struct lexer
{
    char *file_name;
    int fd;
};

struct lexer *lexer_create(char *file_name, int fd);
struct token *next_token(struct lexer *lexer, int *err);
void lexer_destroy(struct lexer *lexer);

#endif /* ! LEXER_H */
