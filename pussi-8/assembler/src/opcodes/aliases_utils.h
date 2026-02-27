#ifndef ALIASES_UTILS_H
#define ALIASES_UTILS_H

#include "parser/instruction.h"

struct instruction *instruction_helper(int line, int real_line, char *opcode,
                                       int args_num, ...);

#endif /* ! ALIASES_UTILS_H */
