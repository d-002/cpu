#ifndef BINARY_H
#define BINARY_H

#include "cli/cli.h"
#include "queue/queue.h"

int to_binary_file(struct cli_args *args, char *path, struct queue *content);

#endif /* ! BINARY_H */
