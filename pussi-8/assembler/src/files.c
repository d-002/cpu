#include "files.h"

#include <stdlib.h>

#include "err.h"
#include "logger.h"
#include "parser/parser.h"
#include "queue.h"

int process_files(struct cli_args *args)
{
    if ((args->run == 0 && args->verbose == 0)
        || args->files_queue->length == 0)
    {
        verbose(args, -1, "No action to take.");
        return SUCCESS;
    }

    while (args->files_queue->length)
    {
        char *path = queue_dequeue(args->files_queue);
        int res = parse_file(args, path);
        free(path);

        if (res)
            return res;
    }

    return SUCCESS;
}
