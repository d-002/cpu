#include "expand_vars.h"

#include <string.h>

#include "logger/logger.h"
#include "utils/errors.h"

int resolve_vars(struct state *state)
{
    int changes;
    do
    {
        changes = 0;

        for (char *key = hash_map_iter_start(state->vars); key;
             key = hash_map_iter_next(state->vars))
        {
            struct token *value = hash_map_get(state->vars, key);
            if (value->type == IDENTIFIER)
            {
                struct token *other = hash_map_get(state->vars, value->data);
                if (other == NULL)
                {
                    if (hash_map_get(state->labels, value->data))
                        continue;

                    logerror(NO_LINE, "Unknown macro name: '%s'", value->data);
                    return VARS_ERROR;
                }

                if (strcmp(value->data, other->data) == 0)
                {
                    logerror(NO_LINE,
                             "Recursive macros are not supported: '%s'",
                             value->data);
                    return VARS_ERROR;
                }

                struct token *other_copy =
                    token_create(other->type, other->data, other->length);
                if (other_copy == NULL)
                {
                    log_alloc_error(NO_LINE);
                    return ALLOC_ERROR;
                }

                struct pair pair = {
                    .key = key,
                    .value = other_copy,
                };

                // should not throw an error
                hash_map_update(state->vars, pair);

                changes = 1;
            }
        }
    } while (changes);

    return SUCCESS;
}

int apply_vars(struct state *state)
{
    for (struct instruction *instruction =
             queue_iter_start(state->instructions);
         instruction; instruction = queue_iter_next(state->instructions))
        for (struct token *token = queue_iter_start(instruction->args_queue);
             token; token = queue_iter_next(instruction->args_queue))
        {
            if (token->type != IDENTIFIER)
                continue;

            struct token *value = hash_map_get(state->vars, token->data);
            if (value == NULL)
            {
                if (hash_map_get(state->labels, token->data))
                    continue;

                logerror(NO_LINE, "Unknown macro or label name: '%s'",
                         token->data);
                return VARS_ERROR;
            }

            struct token *copy =
                token_create(value->type, value->data, value->length);
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

int expand_vars(struct state *state)
{
    int res = resolve_vars(state);
    if (res)
        return res;
    return apply_vars(state);
}
