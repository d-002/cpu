#ifndef STATE_H
#define STATE_H

#include "hash_map.h"
#include "lexer/token.h"
#include "queue.h"

struct label
{
    struct token *token;
    int line; // todo: change these to size_t
};

struct instruction
{
    struct token *opcode;
    struct queue *args_list;
};

struct state
{
    struct queue *instructions;
    struct hash_map *vars;
    struct queue *labels;

    struct token *current_token;

    int line;
};

struct label *label_create(struct token *token, int line);
struct instruction *instruction_create(struct token *opcode);
void label_destroy(struct label *label);
void instruction_destroy(struct instruction *instruction);
struct state *state_create(void);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
