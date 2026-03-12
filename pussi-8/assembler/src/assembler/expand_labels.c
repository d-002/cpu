#include "expand_labels.h"

#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "opcodes/opcodes.h"
#include "utils/errors.h"
#include "utils/numstr.h"

void next_label(struct state *state, char **label_name, int *label_line,
                int start_line)
{
    // since the hash map is unordered, need to manually search for the first
    // label that is after the start line
    *label_name = NULL;

    for (char *name = hash_map_iter_start(state->labels); name;
         name = hash_map_iter_next(state->labels))
    {
        char *label_value = hash_map_get(state->labels, name);
        int line = -atoi(label_value);
        // check if already processed, label at line 0 falls into this case but
        // this should not matter
        if (line <= 0)
            continue;
        if (line < start_line)
            continue;

        if (*label_name == NULL || line < *label_line)
        {
            *label_name = name;
            *label_line = line;
        }
    }
}

int resolve_labels(struct state *state)
{
    int file_line = 0; // inside the file
    int real_line = 0; // instruction index

    char *label_name = NULL;
    int label_line = 0;
    next_label(state, &label_name, &label_line, file_line);

    for (struct instruction *instruction =
             queue_iter_start(state->instructions);
         instruction; instruction = queue_iter_next(state->instructions))
    {
        enum opcodes opcode = get_opcode(instruction);

        // while loop in case multiple labels are defined in the same place
        while (label_name != NULL && file_line == label_line)
        {
            char *encoded_line = my_itoa(real_line);
            if (encoded_line == NULL)
            {
                log_alloc_error(instruction->file_line);
                return ALLOC_ERROR;
            }

            struct pair pair = {
                .key = label_name,
                .value = encoded_line,
            };
            int res = hash_map_update(state->labels, pair);
            if (res != SUCCESS)
                return res;

            // don't stop at the last label, still need to update the
            // real_lines (so can't exit out of the loop early)
            next_label(state, &label_name, &label_line, file_line);
        }

        instruction->real_line = real_line;

        int add = length_from_opcode(opcode);
        file_line++;
        real_line += add;
    }

    // if some labels remain at the end, update their values as well
    for (char *name = hash_map_iter_start(state->labels); name;
         name = hash_map_iter_next(state->labels))
    {
        char *label_value = hash_map_get(state->labels, name);
        int line = -atoi(label_value);
        if (line <= 0)
            continue;

        char *encoded_line = my_itoa(real_line + 1);
        if (encoded_line == NULL)
        {
            log_alloc_error(file_line + 1); // approximation
            return ALLOC_ERROR;
        }

        struct pair pair = {
            .key = name,
            .value = encoded_line,
        };

        int res = hash_map_update(state->labels, pair);
        if (res != SUCCESS)
            return res;
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

            token_destroy(token, true);
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
