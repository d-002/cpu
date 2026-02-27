#ifndef STATE_H
#define STATE_H

#include "hash_map/hash_map.h"
#include "lexer/token.h"

#define NO_REAL_LINE -1

struct instruction
{
    struct token *opcode;
    struct queue *args_queue;

    // line in the program
    int line;
    // effective line in the machine code
    int real_line;
};

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

struct instruction *instruction_create(int line, int real_line,
                                       struct token *opcode);
void instruction_destroy(struct instruction *instruction);
struct state *state_create(void);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
