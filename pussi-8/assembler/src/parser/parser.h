#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <sys/types.h>

#include "cli.h"
#include "state.h"

/**
 * Function used to query a line.
 * Should populate buf_ptr and update n like getline(3),
 * but return 0 on end of file
 */
typedef ssize_t (*line_query)(char **buf_ptr, size_t *n, FILE *stream);

int parse_lines(line_query line_query, FILE *stream, struct state *state,
                char **buf_ptr);
int parse_file(struct cli_args *args, char *path, struct state *state);

#endif /* ! PARSER_H */
