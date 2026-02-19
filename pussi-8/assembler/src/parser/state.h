#ifndef STATE_H
#define STATE_H

#include "hash_map/hash_map.h"
#include "lexer/token.h"

struct instruction
{
    struct token *opcode;
    struct queue *args_queue;
    int line;
};

struct state
{
    struct queue *instructions;
    struct hash_map *vars;
    struct hash_map *labels;

    struct token *current_token;

    int line;
    int line_instr;
};

struct instruction *instruction_create(int line, struct token *opcode);
void instruction_destroy(struct instruction *instruction);
struct state *state_create(void);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
