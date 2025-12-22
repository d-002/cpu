#ifndef BINARY_H
#define BINARY_H

#include "assembler/binary_content.h"
#include "cli.h"

int to_binary_file(struct cli_args *args, char *path,
                   struct binary_content *content);

#endif /* ! BINARY_H */
