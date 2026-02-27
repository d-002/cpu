#include "state.h"

#include <stdlib.h>

#include "instruction.h"

void token_free(void *ptr)
{
    if (ptr == NULL)
        return;

    struct token *token = ptr;
    free(token->data);
    free(token);
}

struct state *state_create(void)
{
    struct state *state = calloc(1, sizeof(struct state));
    if (state == NULL)
        return NULL;

    state->instructions = queue_create();
    state->vars = hash_map_create(token_free);
    state->labels = hash_map_create(free);

    state->current_token = NULL;

    if (state->labels == NULL || state->instructions == NULL
        || state->vars == NULL)
    {
        free(state->instructions);
        free(state->vars);
        return NULL;
    }

    return state;
}

void state_destroy(struct state *state)
{
    while (state->instructions->length)
    {
        struct instruction *instruction = queue_dequeue(state->instructions);
        instruction_destroy(instruction);
    }

    hash_map_destroy(state->labels);
    queue_destroy(state->instructions);
    hash_map_destroy(state->vars);
    token_destroy(state->current_token, 1);
    free(state);
}
