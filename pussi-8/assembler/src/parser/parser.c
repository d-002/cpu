#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "parser.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "parser_utils.h"
#include "state_assignation.h"
#include "state_instruction.h"
#include "utils/errors.h"

int state_start(struct state *state, struct string *string)
{
    int res = get_current_token(state, string, 0);
    if (res != SUCCESS)
        return res;

    switch (state->current_token->type)
    {
    case IDENTIFIER:
        res = state_assignation(state, string);
        if (res != SUCCESS)
            return res;
        break;
    case SPACE:
    case DOT:
        res = state_potential_instruction(state, string);
        if (res != SUCCESS)
            return res;
        break;
    case COMMENT:
        break;
    default:
        unexpected(state->file_line, -1, state->current_token->type);
        return PARSING_ERROR;
    }

    if (state->current_token->type == SPACE)
    {
        res = eat_current_token(state, string, 0, 1);
        if (res != SUCCESS)
            return res;
    }

    if (state->current_token->type == COMMENT)
    {
        res = eat_current_token(state, string, 0, 1);
        if (res != SUCCESS)
            return res;
    }

    if (state->current_token->type == EOL)
        return eat_current_token(state, string, 0, 1);

    unexpected(state->file_line, EOL, state->current_token->type);
    return PARSING_ERROR;
}

int parse_lines(line_query line_query, FILE *stream, struct state *state,
                char **buf_ptr)
{
    size_t n = 0;
    state->file_line = 1;
    state->instr_index = 0;

    while (1)
    {
        ssize_t len = line_query(buf_ptr, &n, stream);
        if (len == 0)
            break;
        if (len < 0)
        {
            logerror(state->file_line, "Could not read line from file: %s.",
                     strerror(errno));
            return IO_ERROR;
        }

        struct string string = {
            .stream = *buf_ptr,
            .length = len,
        };
        int res = state_start(state, &string);
        if (res != SUCCESS)
            return res;

        token_destroy(state->current_token, true);
        state->current_token = NULL;
        state->file_line++;
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

int parse_file(struct cli_args *args, char *path, struct state *state)
{
    verbose(args, NO_LINE, "Parsing '%s'", path);

    FILE *stream = fopen(path, "r");
    if (stream == NULL)
    {
        logerror(NO_LINE, "Failed to open file for reading.", path);
        return IO_ERROR;
    }

    char *buf = NULL;
    int res = parse_lines(get_line, stream, state, &buf);

    free(buf);
    fclose(stream);

    return res;
}
