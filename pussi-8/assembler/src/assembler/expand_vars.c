#include "expand_vars.h"

#include <string.h>

#include "err.h"
#include "logger.h"

int eval_vars(struct state *state)
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
    state++;

    return SUCCESS;
}

int expand_vars(struct state *state)
{
    int res = eval_vars(state);
    if (res)
        return res;
    return apply_vars(state);
}
