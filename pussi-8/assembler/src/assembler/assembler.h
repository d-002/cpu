#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "cli/cli.h"
#include "parser/state.h"

int assemble_file(struct cli_args *args, char *path, struct state *state);

#endif /* ! ASSEMBLER_H */
