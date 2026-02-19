#include "expand_labels.h"

#include <stdlib.h>
#include <string.h>

#include "aliases/aliases.h"
#include "logger/logger.h"
#include "opcodes.h"
#include "utils/errors.h"
#include "utils/numstr.h"

int next_label(struct state *state, char **label_name, int *label_line,
               int first)
{
    *label_name = first ? hash_map_iter_start(state->labels)
                        : hash_map_iter_next(state->labels);
    if (*label_name == NULL)
        return 1;

    char *label_value = hash_map_get(state->labels, *label_name);
    *label_line = atoi(label_value);

    return 0;
}

int resolve_labels(struct state *state)
{
    int line = 0;
    int real_line = 0;

    char *label_name;
    int label_line;
    if (next_label(state, &label_name, &label_line, 1))
        return SUCCESS;

    for (struct instruction *instruction =
             queue_iter_start(state->instructions);
         instruction; instruction = queue_iter_next(state->instructions))
    {
        enum opcodes opcode = get_opcode(instruction);

        if (line == label_line)
        {
            char *encoded_line = itoa(real_line);
            if (encoded_line == NULL)
            {
                log_alloc_error(instruction->line);
                return ALLOC_ERROR;
            }

            struct pair pair = {
                .key = label_name,
                .value = encoded_line,
            };
            int res = hash_map_update(state->labels, pair);
            if (res != SUCCESS)
                return res;

            if (next_label(state, &label_name, &label_line, 0))
                return SUCCESS;
        }

        int add = length_from_opcode(opcode);
        line++;
        real_line += add;
    }

    return SUCCESS;
}

int apply_labels(struct state *state)
{
    for (struct instruction *instruction =
             queue_iter_start(state->instructions);
         instruction; instruction = queue_iter_next(state->instructions))
        for (struct token *token = queue_iter_start(instruction->args_queue);
             token; token = queue_iter_next(instruction->args_queue))
        {
            if (token->type != IDENTIFIER)
                continue;

            char *label = hash_map_get(state->labels, token->data);
            // value should not be NULL

            struct token *copy = token_create(NUMBER_DEC, label, strlen(label));
            if (copy == NULL)
            {
                log_alloc_error(NO_LINE);
                return ALLOC_ERROR;
            }

            token_destroy(token, 1);
            queue_update_at_iter_last(instruction->args_queue, copy);
        }

    return SUCCESS;
}

int expand_labels(struct state *state)
{
    int res = resolve_labels(state);
    if (res != SUCCESS)
        return res;

    return apply_labels(state);
}
