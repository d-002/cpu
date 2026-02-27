#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define NO_REAL_LINE -1

#include "lexer/token.h"

struct instruction
{
    struct token *opcode;
    struct queue *args_queue;

    int file_line;

    // effective line in the machine code
    int real_line;
};

struct instruction *instruction_create(int line, int real_line,
                                       struct token *opcode);
void instruction_destroy(struct instruction *instruction);

#endif /* ! INSTRUCTION_H */
