#include "state_instruction.h"

#include <stdlib.h>

#include "err.h"
#include "logger.h"
#include "parser_utils.h"

int state_label(struct state *state, struct string *string)
{
    if (state->current_token->type != IDENTIFIER)
    {
        unexpected(state->line, IDENTIFIER, state->current_token->type);
        return PARSING_ERROR;
    }

    struct label *label = label_create(state->current_token, state->line);
    if (label == NULL || queue_enqueue(state->labels, label))
    {
        free(label);
        log_alloc_error(state->line);
        return ALLOC_ERROR;
    }

    int res = skip_token(state, string, 0);
    if (res)
        return res;

    if (state->current_token->type == COLON)
    {
        res = eat_current_token(state, string, 0, 1);
        if (res)
            return res;
    }

    return SUCCESS;
}

int state_arguments(struct state *state, struct string *string,
                    struct instruction *instruction)
{
    for (int i = 0; i < 2; i++)
    {
        // comma separator after the first argument, to know if there are any
        // more
        if (i)
        {
            int comma = is_surrounded_type(state, string, COMMA, 0);
            if (comma < 0)
                return -comma;
            if (!comma)
                break;
        }

        if (is_argument_type(state->current_token->type))
        {
            if (queue_enqueue(instruction->args_list, state->current_token))
            {
                log_alloc_error(state->line);
                return ALLOC_ERROR;
            }
            skip_token(state, string, 0);
        }
        else
            return SUCCESS;
    }

    return SUCCESS;
}

int state_instruction(struct state *state, struct string *string)
{
    struct instruction *instruction = instruction_create(state->current_token);
    if (instruction == NULL || queue_enqueue(state->instructions, instruction))
    {
        free(instruction);
        log_alloc_error(state->line);
        return ALLOC_ERROR;
    }

    int res = skip_token(state, string, 0);
    if (res)
        return res;

    // space after opcode if there are any arguments
    res = get_current_token(state, string, 0);
    if (res)
        return res;

    if (state->current_token->type != SPACE)
        return SUCCESS;

    res = eat_current_token(state, string, 0, 1);
    if (res)
        return res;

    // arguments
    return state_arguments(state, string, instruction);
}

int state_potential_instruction(struct state *state, struct string *string)
{
    // optional label parsing
    if (state->current_token->type == DOT)
    {
        int res = eat_current_token(state, string, 0, 1);
        if (res)
            return res;
        res = state_label(state, string);
        if (res)
            return res;
    }

    // space before main instruction part
    if (state->current_token->type == SPACE)
    {
        int res = eat_current_token(state, string, 1, 1);
        if (res)
            return res;
    }
    else
    {
        unexpected(state->line, SPACE, state->current_token->type);
        return PARSING_ERROR;
    }

    // continuing to an instruction
    if (state->current_token->type == OPCODE)
    {
        int res = state_instruction(state, string);
        if (res)
            return res;
    }

    return SUCCESS;
}
