#include "state.h"

#include <stdlib.h>

struct label *label_create(struct token *token, int line)
{
    struct label *label = malloc(sizeof(struct label));
    if (label == NULL)
        return NULL;

    label->token = token;
    label->line = line;

    return label;
}

struct instruction *instruction_create(struct token *opcode)
{
    struct instruction *instruction = malloc(sizeof(struct instruction));
    if (instruction == NULL)
        return NULL;

    instruction->opcode = opcode;
    instruction->args_queue = queue_create();

    if (instruction->args_queue == NULL)
    {
        free(instruction);
        return NULL;
    }

    return instruction;
}

void label_destroy(struct label *label)
{
    token_destroy(label->token, 1);
    free(label);
}

void instruction_destroy(struct instruction *instruction)
{
    token_destroy(instruction->opcode, 1);

    while (instruction->args_queue->length)
        token_destroy(queue_dequeue(instruction->args_queue), 1);
    queue_destroy(instruction->args_queue);

    free(instruction);
}

struct state *state_create(void)
{
    struct state *state = malloc(sizeof(struct state));
    if (state == NULL)
        return NULL;

    state->labels = queue_create();
    state->instructions = queue_create();
    state->vars = hash_map_create();

    state->current_token = NULL;
    state->line = 0;

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
    while (state->labels->length)
    {
        struct label *label = queue_dequeue(state->labels);
        label_destroy(label);
    }

    while (state->instructions->length)
    {
        struct instruction *instruction = queue_dequeue(state->instructions);
        instruction_destroy(instruction);
    }

    queue_destroy(state->labels);
    queue_destroy(state->instructions);
    hash_map_destroy(state->vars);
    token_destroy(state->current_token, 1);
    free(state);
}
