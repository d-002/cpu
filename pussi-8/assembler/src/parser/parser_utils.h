#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include "state.h"
#include "utils/mystring.h"

void expected(int line, enum token_type expected);
void unexpected(int line, int expected, enum token_type got);
int is_argument_type(enum token_type type);
int get_current_token(struct state *state, struct string *string,
                      int expecting_opcode);
int skip_token(struct state *state, struct string *string,
               int expecting_opcode);
int eat_current_token(struct state *state, struct string *string,
                      int expecting_opcode, int empty_data);
int is_surrounded_type(struct state *state, struct string *string,
                       enum token_type central, int expecting_opcode);

#endif /* ! PARSER_UTILS_H */
