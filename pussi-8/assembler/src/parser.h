#ifndef PARSER_H
#define PARSER_H

#include "cli.h"
#include "hash_map.h"
#include "lexer/token.h"
#include "queue.h"

struct line
{
    struct token *opcode;
    struct queue *args_list;
};

struct state
{
    struct queue *lines;
    struct hash_map *vars;
};

int parse_file(struct cli_args *args, char *path);

#endif /* ! PARSER_H */
