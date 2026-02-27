#ifndef TO_MACHINE_H
#define TO_MACHINE_H

#include "cli/cli.h"
#include "parser/state.h"
#include "queue/queue.h"

int to_machine_code(struct cli_args *args, struct state *state,
                    struct queue *queue, struct queue *temp_content,
                    struct queue *content);

#endif /* ! TO_MACHINE_H */
