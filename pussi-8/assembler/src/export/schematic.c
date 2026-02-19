#include "schematic.h"

#include <stdlib.h>

#include "export_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"

int to_schematic(struct cli_args *args, char *path, struct queue *content)
{
    char *export_path = change_extension(path, "schematic");
    if (export_path == NULL)
        return ALLOC_ERROR;

    verbose(args, NO_LINE, "Exporting schematic to '%s'", export_path);
    content++;

    free(export_path);
    return SUCCESS;
}
