#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include "assembler/binary_content.h"
#include "cli.h"

int to_schematic(struct cli_args *args, char *path,
                 struct binary_content *content);

#endif /* ! SCHEMATIC_H */
