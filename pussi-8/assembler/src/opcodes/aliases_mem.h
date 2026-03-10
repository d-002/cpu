#ifndef ALIASES_MEM_H
#define ALIASES_MEM_H

#include "parser/instruction.h"

int handle_mov(struct instruction *instruction, struct queue *out);
int handle_ldi_2(struct instruction *instruction, struct queue *out);

#endif /* ! ALIASES_MEM_H */
