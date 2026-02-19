#include "files.h"

#include <stdlib.h>

#include "assembler/assembler.h"
#include "logger/logger.h"
#include "parser/parser.h"
#include "queue/queue.h"
#include "utils/errors.h"

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
        struct state *state = state_create();
        if (state == NULL)
        {
            log_alloc_error(-1);
            return ALLOC_ERROR;
        }

        char *path = queue_dequeue(args->files_queue);
        int res = parse_file(args, path, state);
        if (!res)
            res = assemble_file(args, path, state);

        free(path);
        state_destroy(state);

        if (res)
            return res;
    }

    return SUCCESS;
}
