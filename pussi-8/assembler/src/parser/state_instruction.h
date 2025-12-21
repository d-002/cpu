#ifndef STATE_INSTRUCTION_H
#define STATE_INSTRUCTION_H

#include "mystring.h"
#include "state.h"

int state_label(struct state *state, struct string *string);
int state_arguments(struct state *state, struct string *string,
                    struct instruction *instruction);
int state_instruction(struct state *state, struct string *string);
int state_potential_instruction(struct state *state, struct string *string);

#endif /* ! STATE_INSTRUCTION_H */
