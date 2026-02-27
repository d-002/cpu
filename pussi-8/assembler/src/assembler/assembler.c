#include "assembler.h"

#include <stdio.h>

#include "expand_labels.h"
#include "expand_vars.h"
#include "export/binary.h"
#include "export/schematic.h"
#include "logger/logger.h"
#include "parser/instruction.h"
#include "to_machine.h"
#include "utils/errors.h"

int assemble_file(struct cli_args *args, char *path, struct state *state)
{
    verbose(args, NO_LINE, "Expanding variables");
    int res = expand_vars(state);
    if (res != SUCCESS)
        return res;

    verbose(args, NO_LINE, "Expanding labels");
    res = expand_labels(state);
    if (res != SUCCESS)
        return res;

    verbose(args, NO_LINE, "Assembling");

    struct queue *queue = queue_create();
    struct queue *content = queue_create();
    if (queue == NULL || content == NULL)
    {
        queue_destroy(queue);
        queue_destroy(content);
        log_alloc_error(NO_LINE);
        return ALLOC_ERROR;
    }

    res = to_machine_code(args, state, queue, content);
    if (args->run)
    {
        if (!res)
            res = to_binary_file(args, path, content);
        if (!res)
            res = to_schematic(args, path, content);
    }

    while (queue->length)
        instruction_destroy(queue_dequeue(queue));
    queue_destroy(queue);
    queue_destroy(content);
    return res;
}
