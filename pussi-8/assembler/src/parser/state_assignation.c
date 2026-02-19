#include "state_assignation.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "parser_utils.h"
#include "utils/errors.h"

int state_assignation(struct state *state, struct string *string)
{
    char *key = state->current_token->data;
    int res = eat_current_token(state, string, 0, 0);
    if (res != SUCCESS)
    {
        free(key);
        return res;
    }

    if (hash_map_get(state->labels, key))
    {
        logerror(state->line,
                 "Cannot have a label and a macro with the same name");
        return VARS_ERROR;
    }

    int equal_sign = is_surrounded_type(state, string, EQUAL_SIGN, 0);
    if (equal_sign < 0)
    {
        free(key);
        return -equal_sign;
    }
    if (!equal_sign)
    {
        expected(state->line, EQUAL_SIGN);
        free(key);
        return PARSING_ERROR;
    }

    if (!is_argument_type(state->current_token->type))
    {
        unexpected(state->line, -1, state->current_token->type);
        free(key);
        return PARSING_ERROR;
    }

    struct token *value = state->current_token;

    struct pair pair = {
        .key = key,
        .value = value,
    };
    res = hash_map_insert(state->vars, pair);
    if (res != SUCCESS)
    {
        int err;
        if (res == HASH_MAP_DUPE_ERROR)
        {
            logerror(state->line, "Duplicate macro name: '%s'", key);
            err = VARS_ERROR;
        }
        else
        {
            log_alloc_error(state->line);
            err = ALLOC_ERROR;
        }
        free(key);
        return err;
    }

    res = skip_token(state, string, 0);
    if (res != SUCCESS)
        return res;

    return SUCCESS;
}
