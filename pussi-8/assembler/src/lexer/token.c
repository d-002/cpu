#include "token.h"

#include <stdlib.h>
#include <string.h>

char *token_names[] = {
    "end of line",
    "space",
    "identifier",
    "number (bin format)",
    "number (hex format)",
    "number",
    "register",
    "memory",
    "port",
    "comment",
    "opcode",

    "single dot",
    "comma",
    "colon",
    "equal sign",

    "number",
};

struct token *token_create(enum token_type type, char *stream, size_t len)
{
    struct token *token = malloc(sizeof(struct token));
    if (token == NULL)
        return NULL;

    token->type = type;
    char *data = calloc(len + 1, sizeof(char));

    if (data == NULL)
    {
        free(token);
        return NULL;
    }

    memcpy(data, stream, len);
    token->data = data;
    token->length = len;

    return token;
}

struct token *token_dup(struct token *old)
{
    return token_create(old->type, old->data, old->length);
}

void token_destroy(struct token *token, bool empty_data)
{
    if (token == NULL)
        return;

    if (empty_data)
        free(token->data);
    free(token);
}

char *type2name(enum token_type type)
{
    return token_names[type];
}
