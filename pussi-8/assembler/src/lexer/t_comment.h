#ifndef T_COMMENT_H
#define T_COMMENT_H

#include "token.h"

int token_comment(char *stream, int line, size_t len, struct token *out);

#endif /* ! T_COMMENT_H */
