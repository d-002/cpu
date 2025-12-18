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

#define BUF_SIZE 1024

int state_create(struct state *state)
{
    state->lines = queue_create();
    state->vars = hash_map_create();

    if (state->lines == NULL || state->vars == NULL)
    {
        free(state->lines);
        free(state->vars);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}

void state_destroy(struct state *state)
{
    while (!queue_isempty(state->lines))
    {
        struct line *line = queue_dequeue(state->lines);

        token_destroy(line->opcode);

        while (!queue_isempty(line->args_list))
            token_destroy(queue_dequeue(line->args_list));
        queue_destroy(line->args_list);
    }

    queue_destroy(state->lines);
    hash_map_destroy(state->vars);
}

int parse_line(struct state *state, struct cli_args *args, struct string string,
               int line)
{
    int expecting_opcode = 0;
    state++;

    while (string.len)
    {
        struct token *token;
        int res = next_token(string, line, expecting_opcode, &token);
        if (res)
            return res;

        verbose(args, line, "Token: '%s', type %d", token->data, token->type);

        string.stream += token->length;
        string.len -= token->length;
        token_destroy(token);
    }

    return SUCCESS;
}

int parse_file(struct cli_args *args, char *path)
{
    verbose(args, NO_LINE, "Assembling file %s...", path);

    struct state state;
    int res = state_create(&state);
    if (res)
        return res;

    FILE *stream = fopen(path, "r");
    if (stream == NULL)
    {
        logerror(NO_LINE, "Failed to open file for reading.", path);
        return IO_ERROR;
    }

    char *buf = NULL;
    size_t n = 0;
    int line = 1;

    while (1)
    {
        ssize_t len = getline(&buf, &n, stream);
        if (len < 0)
        {
            if (feof(stream))
                break;
            else
            {
                logerror(line, "Could not read line from file: %s.",
                         strerror(errno));
                state_destroy(&state);
                free(buf);
                fclose(stream);
                return IO_ERROR;
            }
        }

        struct string string = {
            .stream = buf,
            .len = len,
        };
        int res = parse_line(&state, args, string, line);
        if (res)
        {
            state_destroy(&state);
            free(buf);
            fclose(stream);
            return res;
        }

        line++;
    }

    state_destroy(&state);
    free(buf);
    fclose(stream);
    return SUCCESS;
}
