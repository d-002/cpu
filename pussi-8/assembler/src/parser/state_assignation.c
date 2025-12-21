#include "state_assignation.h"

#include <stdlib.h>

#include "err.h"
#include "logger.h"
#include "parser_utils.h"

int state_assignation(struct state *state, struct string *string)
{
    char *key = state->current_token->data;
    int res = eat_current_token(state, string, 0, 0);
    if (res)
    {
        free(key);
        return res;
    }

    int comma = is_surrounded_type(state, string, EQUAL_SIGN, 0);
    if (comma < 0)
    {
        free(key);
        return -comma;
    }
    if (!comma)
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

    char *value = state->current_token->data;
    res = eat_current_token(state, string, 0, 0);
    if (res)
        return res;

    struct pair pair = {
        .key = key,
        .value = value,
    };
    res = hash_map_insert(state->vars, pair);
    if (res == HASHMAP_DUPE_ERROR)
    {
        logerror(state->line, "Duplicate macro name: '%s'", key);
        free(key);
        free(value);
        return PARSING_ERROR;
    }
    if (res)
    {
        log_alloc_error(state->line);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}
