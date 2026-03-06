#include "binary.h"

#include <stdio.h>
#include <stdlib.h>

#include "export_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"

int to_binary_file(struct cli_args *args, char *path, struct queue *content)
{
    char *export_path = change_extension(path, "bin");
    FILE *f = NULL;
    int err = SUCCESS;

    if (export_path == NULL)
        return ALLOC_ERROR;

    verbose(args, NO_LINE, "Exporting binary encoded program to '%s'.",
            export_path);

    f = fopen(export_path, "w");
    if (f == NULL)
    {
        logerror(NO_LINE, "Failed to open file.");
        err = IO_ERROR;
        goto end;
    }

    // important to not dequeue here, to allow exporting to multiple formats
    for (short *instruction = queue_iter_start(content); instruction;
         instruction = queue_iter_next(content))
    {
        char buf[2] = { *instruction >> 8, *instruction & 255 };
        int wres = fwrite(buf, sizeof(char), 2, f);

        if (wres <= 0)
        {
            logerror(NO_LINE, "Failed to write to file.");
            err = IO_ERROR;
            goto end;
        }
    }

end:
    fclose(f);
    free(export_path);
    return err;
}
