#ifndef LIST_INSTRUCTIONS_H
#define LIST_INSTRUCTIONS_H

#include "cli.h"
#include "parser/state.h"
#include "queue.h"

int to_machine_code(struct cli_args *args, struct state *state,
                    struct queue *queue, struct queue *content);

#endif /* ! LIST_INSTRUCTIONS_H */
