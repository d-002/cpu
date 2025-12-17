#include "parser.h"

#include "err.h"
#include "lexer.h"
#include "logger.h"

int parse_file(struct cli_args *args, char *file_name) {
    verbose(args, "Assembling file %s...", file_name);

    return SUCCESS;
}
