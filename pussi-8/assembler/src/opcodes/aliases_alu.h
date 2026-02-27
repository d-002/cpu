#ifndef ALIASES_ALU_H
#define ALIASES_ALU_H

#include "parser/instruction.h"
#include "queue/queue.h"

int handle_custom_calc(struct instruction *instruction, struct queue *out,
                       char *opcode);

#endif /* ! ALIASES_ALU_H */
