#include "state.h"

#include <ctype.h>
#include <stdlib.h>

struct instruction *instruction_create(int line, int real_line,
                                       struct token *opcode)
{
    struct instruction *instruction = malloc(sizeof(struct instruction));
    if (instruction == NULL)
        return NULL;

    instruction->opcode = opcode;
    instruction->args_queue = queue_create();
    instruction->line = line;
    instruction->real_line = real_line;

    if (instruction->args_queue == NULL)
    {
        free(instruction);
        return NULL;
    }

    // make the opcode uppercase for comparisons later
    for (size_t i = 0; i < opcode->length; i++)
        opcode->data[i] = toupper(opcode->data[i]);

    return instruction;
}

void instruction_destroy(struct instruction *instruction)
{
    token_destroy(instruction->opcode, 1);

    while (instruction->args_queue->length)
        token_destroy(queue_dequeue(instruction->args_queue), 1);
    queue_destroy(instruction->args_queue);

    free(instruction);
}

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
