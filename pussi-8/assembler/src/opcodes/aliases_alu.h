#ifndef ALIASES_ALU_H
#define ALIASES_ALU_H

#include "parser/instruction.h"
#include "queue/queue.h"

int handle_custom_calc2(struct instruction *instruction, struct queue *out,
                        char *opcode);
int handle_custom_calc3(struct instruction *instruction, struct queue *out,
                        char *opcode);
int handle_test(struct instruction *instruction, struct queue *out);

#endif /* ! ALIASES_ALU_H */
