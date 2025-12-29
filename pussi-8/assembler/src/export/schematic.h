#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include "cli.h"
#include "queue.h"

int to_schematic(struct cli_args *args, char *path, struct queue *content);

#endif /* ! SCHEMATIC_H */
