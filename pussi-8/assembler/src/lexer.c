#include "lexer.h"

#include <stdlib.h>

#include "err.h"

struct lexer *lexer_create(char *file_name, int fd)
{
    struct lexer *lexer = malloc(sizeof(struct lexer));
    if (lexer == NULL)
        return NULL;

    lexer->file_name = file_name;
    lexer->fd = fd;

    return lexer;
}

struct token *next_token(struct lexer *lexer, int *err)
{
    if (err)
        err = SUCCESS;

    lexer++;
    return NULL;
}

void lexer_destroy(struct lexer *lexer)
{
    free(lexer);
}
