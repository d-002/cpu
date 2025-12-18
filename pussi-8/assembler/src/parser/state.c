#include "state.h"

#include <stdlib.h>

#include "err.h"

int state_create(struct state *state)
{
    state->lines = queue_create();
    state->vars = hash_map_create();

    if (state->lines == NULL || state->vars == NULL)
    {
        free(state->lines);
        free(state->vars);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}

void state_destroy(struct state *state)
{
    while (!queue_isempty(state->lines))
    {
        struct line *line = queue_dequeue(state->lines);

        token_destroy(line->opcode);

        while (!queue_isempty(line->args_list))
            token_destroy(queue_dequeue(line->args_list));
        queue_destroy(line->args_list);
    }

    queue_destroy(state->lines);
    hash_map_destroy(state->vars);
}
