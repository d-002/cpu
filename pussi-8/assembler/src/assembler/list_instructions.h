#ifndef LIST_INSTRUCTIONS_H
#define LIST_INSTRUCTIONS_H

#include "parser/state.h"
#include "queue.h"

int to_machine_code(struct state *state, struct queue *queue,
                    struct queue *content);

#endif /* ! LIST_INSTRUCTIONS_H */
