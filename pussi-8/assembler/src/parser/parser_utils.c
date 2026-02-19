#include "parser_utils.h"

#include "lexer/lexer.h"
#include "logger/logger.h"
#include "utils/errors.h"

void expected(int line, enum token_type expected)
{
    logerror(line, "Expected %s but could not find it", type2name(expected));
}

void unexpected(int line, int expected, enum token_type got)
{
    if (expected < 0)
        logerror(line, "Unexpected %s", type2name(got));
    else
        logerror(line, "Expected %s, got %s", type2name(expected),
                 type2name(got));
}

int is_argument_type(enum token_type type)
{
    switch (type)
    {
    case NUMBER_BIN:
    case NUMBER_HEX:
    case NUMBER_DEC:
    case REGISTER:
    case MEMORY:
    case PORT:
    case IDENTIFIER:
        return 1;
    default:
        return 0;
    }
}

int get_current_token(struct state *state, struct string *string,
                      bool expecting_opcode)
{
    if (state->current_token == NULL)
    {
        int res = next_token(string, state->line, expecting_opcode,
                             &(state->current_token));

        if (res != SUCCESS)
            return res;
    }

    return SUCCESS;
}

int skip_token(struct state *state, struct string *string,
               bool expecting_opcode)
{
    size_t len = state->current_token->length;
    string->length -= len;
    string->stream += len;

    state->current_token = NULL;

    return get_current_token(state, string, expecting_opcode);
}

int eat_current_token(struct state *state, struct string *string,
                      bool expecting_opcode, int empty_data)
{
    struct token *current = state->current_token;
    int res = skip_token(state, string, expecting_opcode);
    token_destroy(current, empty_data);

    return res;
}

int is_surrounded_type(struct state *state, struct string *string,
                       enum token_type central, bool expecting_opcode)
{
    for (int step = 0; step < 3;)
    {
        int res = get_current_token(state, string, 0);
        if (res != SUCCESS)
            return -res;

        if (step != 1 && state->current_token->type == SPACE)
        {
            eat_current_token(state, string, expecting_opcode, 1);
            step++;
        }
        else if (step < 2 && state->current_token->type == central)
        {
            eat_current_token(state, string, expecting_opcode, 1);
            step = 2;
        }
        else
            return step == 2;
    }

    return 1;
}
