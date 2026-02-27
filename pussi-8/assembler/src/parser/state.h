#ifndef STATE_H
#define STATE_H

#include "hash_map/hash_map.h"
#include "lexer/token.h"

struct state
{
    struct queue *instructions;
    struct hash_map *vars;
    struct hash_map *labels;

    struct token *current_token;

    // line in the program
    int line;
    // instruction index from the start of the program
    int line_instr;
};

struct state *state_create(void);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
