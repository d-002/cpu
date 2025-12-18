#include "state.h"

#include <stdlib.h>

struct state *state_create(void)
{
    struct state *state = malloc(sizeof(struct state));
    if (state == NULL)
        return 0;

    state->lines = queue_create();
    state->vars = hash_map_create();

    state->current_token = NULL;

    if (state->lines == NULL || state->vars == NULL)
    {
        free(state->lines);
        free(state->vars);
        return NULL;
    }

    return state;
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
    token_destroy(state->current_token);
    free(state);
}
