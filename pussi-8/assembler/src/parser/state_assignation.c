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
    res = skip_token(state, string, 0);
    if (res)
        return res;

    struct pair pair = {
        .key = key,
        .value = value,
    };
    res = hash_map_insert(state->vars, pair);
    if (res == HASH_MAP_DUPE_ERROR)
    {
        logerror(state->line, "Duplicate macro name: '%s'", key);
        free(key);
        token_destroy(value, 1);
        return PARSING_ERROR;
    }
    if (res)
    {
        log_alloc_error(state->line);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}
