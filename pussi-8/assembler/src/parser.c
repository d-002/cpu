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

int parse_line(struct cli_args *args, struct string string, int line)
{
    int expecting_opcode = 0;

    while (string.len)
    {
        struct token token;
        int res = next_token(string, line, expecting_opcode, &token);
        if (res)
            return res;

        verbose(args, line, "Token: '%s', type %d", token.data, token.type);
        free(token.data);

        string.stream += token.length;
        string.len -= token.length;
    }

    return SUCCESS;
}

int parse_file(struct cli_args *args, char *path)
{
    verbose(args, NO_LINE, "Assembling file %s...", path);

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
                free(buf);
                fclose(stream);
                return IO_ERROR;
            }
        }

        struct string string = {
            .stream = buf,
            .len = len,
        };
        int res = parse_line(args, string, line);
        if (res)
        {
            free(buf);
            fclose(stream);
            return res;
        }

        line++;
    }

    free(buf);
    fclose(stream);
    return SUCCESS;
}
