#ifndef ALIASES_IO_H
#define ALIASES_IO_H

#include "parser/instruction.h"

int handle_in_2(struct instruction *instruction, struct queue *out);
int handle_out_2(struct instruction *instruction, struct queue *out);

#endif /* ! ALIASES_IO_H */
