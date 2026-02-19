#ifndef T_COMMENT_H
#define T_COMMENT_H

#include "token.h"
#include "utils/mystring.h"

int token_comment(struct string *string, int line, struct token **out);

#endif /* ! T_COMMENT_H */
