#include "binary.h"

#include <stdio.h>
#include <stdlib.h>

#include "export_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"

int to_binary_file(struct cli_args *args, char *path, struct queue *content)
{
    char *export_path = change_extension(path, "bin");
    if (export_path == NULL)
        return ALLOC_ERROR;

    verbose(args, NO_LINE, "Exporting binary encoded program to '%s'.",
            export_path);

    FILE *f = fopen(export_path, "w");
    if (f == NULL)
    {
        logerror(NO_LINE, "Failed to write to file.");
        return ALLOC_ERROR;
    }

    free(export_path);
    return SUCCESS;
}
