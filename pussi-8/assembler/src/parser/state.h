#ifndef STATE_H
#define STATE_H

#include "hash_map/hash_map.h"
#include "lexer/token.h"

struct state
{
    // WARNING: not freed in this structure
    char *file_name;

    struct queue *instructions;
    struct hash_map *vars;
    struct hash_map *labels;

    struct token *current_token;

    int file_line;
    int instr_index;
};

struct state *state_create(char *file_name);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
