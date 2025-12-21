#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "parser.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "lexer/lexer.h"
#include "logger.h"
#include "mystring.h"
#include "state.h"

#define BUF_SIZE 1024

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
                      int expecting_opcode)
{
    if (state->current_token == NULL)
    {
        int res = next_token(string, state->line, expecting_opcode,
                             &(state->current_token));

        if (res)
            return res;
    }

    return SUCCESS;
}

int skip_token(struct state *state, struct string *string, int expecting_opcode)
{
    size_t len = state->current_token->length;
    string->length -= len;
    string->stream += len;

    state->current_token = NULL;

    return get_current_token(state, string, expecting_opcode);
}

int eat_current_token(struct state *state, struct string *string,
                      int expecting_opcode, int empty_data)
{
    struct token *current = state->current_token;
    int res = skip_token(state, string, expecting_opcode);
    token_destroy(current, empty_data);

    return res;
}

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

int is_surrounded_type(struct state *state, struct string *string,
                       enum token_type central, int expecting_opcode)
{
    for (int step = 0; step < 3;)
    {
        int res = get_current_token(state, string, 0);
        if (res)
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

    if (state->current_token->type == SPACE)
    {
        int res = eat_current_token(state, string, 0, 1);
        if (res)
            return res;
    }

    return SUCCESS;
}

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

int state_start(struct state *state, struct string *string)
{
    int res = get_current_token(state, string, 0);
    if (res)
        return res;

    switch (state->current_token->type)
    {
    case IDENTIFIER:
        res = state_assignation(state, string);
        if (res)
            return res;
        break;
    case SPACE:
    case DOT:
        res = state_potential_instruction(state, string);
        if (res)
            return res;
        break;
    case COMMENT:
        break;
    default:
        unexpected(state->line, -1, state->current_token->type);
        return PARSING_ERROR;
    }

    if (state->current_token->type == SPACE)
    {
        res = eat_current_token(state, string, 0, 1);
        if (res)
            return res;
    }

    if (state->current_token->type == COMMENT)
    {
        res = eat_current_token(state, string, 0, 1);
        if (res)
            return res;
    }

    if (state->current_token->type == EOL)
        return eat_current_token(state, string, 0, 1);

    unexpected(state->line, EOL, state->current_token->type);
    return PARSING_ERROR;
}

int parse_lines(line_query line_query, FILE *stream, struct state *state,
                char **buf_ptr)
{
    size_t n = 0;
    state->line = 1;

    while (1)
    {
        ssize_t len = line_query(buf_ptr, &n, stream);
        if (len == 0)
            break;
        if (len < 0)
        {
            logerror(state->line, "Could not read line from file: %s.",
                     strerror(errno));
            return IO_ERROR;
        }

        struct string string = {
            .stream = *buf_ptr,
            .length = len,
        };
        int res = state_start(state, &string);
        if (res)
            return res;

        if (string.length)
        {
            unexpected(state->line, EOL, state->current_token->type);
            return PARSING_ERROR;
        }

        token_destroy(state->current_token, 1);
        state->current_token = NULL;
        state->line++;
    }

    return SUCCESS;
}

ssize_t get_line(char **buf_ptr, size_t *n, FILE *stream)
{
    ssize_t res = getline(buf_ptr, n, stream);
    if (res >= 0)
        return res;

    if (feof(stream))
        return 0;

    return res;
}

int parse_file(struct cli_args *args, char *path)
{
    verbose(args, NO_LINE, "Parsing '%s'", path);

    struct state *state = state_create();
    if (state == NULL)
    {
        log_alloc_error(-1);
        return ALLOC_ERROR;
    }

    FILE *stream = fopen(path, "r");
    if (stream == NULL)
    {
        logerror(NO_LINE, "Failed to open file for reading.", path);
        return IO_ERROR;
    }

    char *buf = NULL;
    int res = parse_lines(get_line, stream, state, &buf);

    state_destroy(state);
    free(buf);
    fclose(stream);

    return res;
}
