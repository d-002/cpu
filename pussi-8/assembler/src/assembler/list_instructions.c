#include "list_instructions.h"

#include "err.h"

int add_next_instruction(struct state *state, struct queue *queue, int first)
{
    state++;
    queue++;
    first++;

    return SUCCESS;
}
