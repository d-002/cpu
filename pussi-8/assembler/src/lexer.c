#include "lexer.h"

#include <stdlib.h>

#include "err.h"

int next_token(int fd, struct token *out) {
    fd++;
    out++;

    return SUCCESS;
}
