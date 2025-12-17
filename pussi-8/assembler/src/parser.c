#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "parser.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "lexer.h"
#include "logger.h"

#define BUF_SIZE 1024

int parse_line(char *line, size_t len)
{
    line++;
    len++;

    return SUCCESS;
}

int parse_file(struct cli_args *args, char *path)
{
    verbose(args, "Assembling file %s...", path);

    FILE *stream = fopen(path, "r");
    if (stream == NULL)
    {
        logerror("Failed to open file for reading.", path);
        return IO_ERROR;
    }

    char *buf = NULL;
    ;
    size_t n = 0;

    while (1)
    {
        ssize_t len = getline(&buf, &n, stream);
        if (len < 0)
        {
            if (feof(stream))
                break;
            else
            {
                logerror("Could not read line from file: %s.", strerror(errno));
                free(buf);
                fclose(stream);
                return IO_ERROR;
            }
        }

        int res = parse_line(buf, len);
        if (res)
        {
            free(buf);
            fclose(stream);
            return res;
        }
    }

    free(buf);
    fclose(stream);
    return SUCCESS;
}
