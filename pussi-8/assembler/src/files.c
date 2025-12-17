#include "files.h"

#include <stdlib.h>

#include "err.h"
#include "parser.h"
#include "logger.h"
#include "queue.h"

int process_files(struct cli_args *args)
{
    if ((args->run == 0 && args->verbose == 0)
        || queue_isempty(args->files_queue))
    {
        verbose(args, "No action to take.");
        return SUCCESS;
    }

    while (!queue_isempty(args->files_queue))
    {
        char *path = queue_dequeue(args->files_queue);
        int res = parse_file(args, path);
        free(path);

        if (res)
            return res;
    }

    return SUCCESS;
}
