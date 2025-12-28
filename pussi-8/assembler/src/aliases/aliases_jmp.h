#ifndef ALIASES_JMP_H
#define ALIASES_JMP_H

#include "parser/state.h"

int handle_custom_jump(struct instruction *instruction, struct queue *out,
                       char *cond_code);

#endif /* ! ALIASES_JMP_H */
