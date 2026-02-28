#include "files.h"

#include <stdlib.h>

#include "assembler/assembler.h"
#include "logger/logger.h"
#include "parser/parser.h"
#include "queue/queue.h"
#include "utils/errors.h"

int process_files(struct cli_args *args)
{
    if ((!args->run && !args->print) || args->files_queue->length == 0)
    {
        verbose(args, -1, "Nothing to do.");
        return SUCCESS;
    }

    while (args->files_queue->length)
    {
        char *path = queue_dequeue(args->files_queue);
        struct state *state = state_create(path);
        if (state == NULL)
        {
            free(path);
            log_alloc_error(-1);
            return ALLOC_ERROR;
        }

        int res = parse_file(args, path, state);
        if (!res)
            res = assemble_file(args, path, state);

        free(path);
        state_destroy(state);

        if (res != SUCCESS)
            return res;
    }

    return SUCCESS;
}
