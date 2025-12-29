#include "assembler.h"

#include <stdio.h>

#include "err.h"
#include "expand_labels.h"
#include "expand_vars.h"
#include "export/binary.h"
#include "export/schematic.h"
#include "list_instructions.h"
#include "logger.h"
int assemble_file(struct cli_args *args, char *path, struct state *state)
{
    verbose(args, NO_LINE, "Expanding variables");
    int res = expand_vars(state);
    if (res)
        return res;

    verbose(args, NO_LINE, "Expanding labels");
    res = expand_labels(state);
    if (res)
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

    res = to_machine_code(state, queue, content);
    if (!res)
        res = to_binary_file(args, path, content);

    if (!res)
        res = to_schematic(args, path, content);

    queue_destroy(queue);
    queue_destroy(content);
    return res;
}
