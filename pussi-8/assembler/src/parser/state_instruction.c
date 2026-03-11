#include "state_instruction.h"

#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "parser_utils.h"
#include "utils/errors.h"
#include "utils/numstr.h"

int state_label(struct state *state, struct string *string)
{
    if (state->current_token->type != IDENTIFIER)
    {
        unexpected(state->file_line, IDENTIFIER, state->current_token->type);
        return PARSING_ERROR;
    }

    if (hash_map_get(state->vars, state->current_token->data))
    {
        logerror(state->file_line,
                 "Cannot have a label and a macro with the same name.");
        return VARS_ERROR;
    }

    // start with the labels having the line they appeared at as value
    char *key = malloc((state->current_token->length + 1) * sizeof(char));
    // store lines as negative to distinguish between before and after solve
    char *value = itoa(-state->instr_index);
    if (key == NULL || value == NULL)
    {
        free(key);
        free(value);
        log_alloc_error(state->file_line);
        return ALLOC_ERROR;
    }
    memcpy(key, state->current_token->data, state->current_token->length + 1);

    struct pair pair = {
        .key = key,
        .value = value,
    };
    int err = hash_map_insert(state->labels, pair);
    if (err != SUCCESS)
    {
        if (err == HASH_MAP_DUPE_ERROR)
        {
            logerror(state->file_line, "Duplicate label name: '%s'.", key);
            err = VARS_ERROR;
        }
        else
        {
            log_alloc_error(state->file_line);
            err = ALLOC_ERROR;
        }
        free(key);
        free(value);
        return err;
    }

    err = eat_current_token(state, string, 0, 1);
    if (err != SUCCESS)
        return err;

    if (state->current_token->type == COLON)
    {
        err = eat_current_token(state, string, 0, 1);
        if (err != SUCCESS)
            return err;
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
            if (queue_enqueue(instruction->args_queue, state->current_token))
            {
                log_alloc_error(state->file_line);
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
    struct instruction *instruction = instruction_create(
        state->file_line, NO_REAL_LINE, state->current_token);
    if (instruction == NULL || queue_enqueue(state->instructions, instruction))
    {
        free(instruction);
        log_alloc_error(state->file_line);
        return ALLOC_ERROR;
    }

    int err = skip_token(state, string, 0);
    if (err != SUCCESS)
        return err;

    // space after opcode if there are any arguments
    err = get_current_token(state, string, 0);
    if (err != SUCCESS)
        return err;

    if (state->current_token->type != SPACE)
        return SUCCESS;

    err = eat_current_token(state, string, 0, 1);
    if (err != SUCCESS)
        return err;

    // arguments
    err = state_arguments(state, string, instruction);
    state->instr_index++;
    return err;
}

int state_potential_instruction(struct state *state, struct string *string)
{
    // optional label parsing
    if (state->current_token->type == DOT)
    {
        int err = eat_current_token(state, string, 0, 1);
        if (err != SUCCESS)
            return err;
        err = state_label(state, string);
        if (err != SUCCESS)
            return err;
    }

    // space before main instruction part
    if (state->current_token->type == SPACE)
    {
        int err = eat_current_token(state, string, 1, 1);
        if (err != SUCCESS)
            return err;
    }
    else
    {
        unexpected(state->file_line, SPACE, state->current_token->type);
        return PARSING_ERROR;
    }

    // continuing to an instruction
    if (state->current_token->type == OPCODE)
    {
        int err = state_instruction(state, string);
        if (err != SUCCESS)
            return err;
    }

    return SUCCESS;
}
