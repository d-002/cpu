#ifndef STATE_H
#define STATE_H

#include "hash_map.h"
#include "lexer/token.h"
#include "queue.h"

struct line
{
    struct token *opcode;
    struct queue *args_list;
};

struct state
{
    struct queue *lines;
    struct hash_map *vars;

    struct token *current_token;
};

struct state *state_create(void);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
