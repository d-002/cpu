#ifndef TO_MACHINE_UTILS_H
#define TO_MACHINE_UTILS_H

#include "opcodes/opcodes.h"

int to_machine_i_jumps(struct instruction *instruction, enum opcodes opcode,
                       struct queue *content);
int to_machine_i_data(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content);
int to_machine_i_calc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content);
int to_machine_i_misc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content);

#endif /* ! TO_MACHINE_UTILS_H */
