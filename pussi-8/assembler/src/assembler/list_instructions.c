#include "list_instructions.h"

#include "err.h"

int add_next_instruction(struct state *state, struct queue *queue,
                         size_t *index)
{
    state++;
    queue++;
    (*index)++;

    return SUCCESS;
}
