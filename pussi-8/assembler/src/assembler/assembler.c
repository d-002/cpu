#include "assembler.h"

#include <stdio.h>

#include "binary_content.h"
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
    struct binary_content *content = binary_content_create();
    if (queue == NULL || content == NULL)
    {
        queue_destroy(queue);
        binary_content_destroy(content);
        log_alloc_error(NO_LINE);
        return ALLOC_ERROR;
    }

    while (1)
    {
        res = add_next_instruction(state, queue);

        if (res)
        {
            queue_destroy(queue);
            binary_content_destroy(content);
            return res;
        }

        if (queue->length == 0)
            break;

        while (queue->length)
        {
            struct instruction *instruction = queue_dequeue(queue);
            printf("%s ", instruction->opcode->data);
            int first = 1;
            for (struct token *arg = queue_iter_start(instruction->args_queue);
                 arg; arg = queue_iter_next(instruction->args_queue))
            {
                if (first)
                    first = 0;
                else
                    putchar(',');
                printf("%s", arg->data);
            }
            putchar('\n');
            instruction_destroy(instruction);
        }
    }

    res = to_binary_file(args, path, content);
    if (!res)
        res = to_schematic(args, path, content);

    queue_destroy(queue);
    binary_content_destroy(content);
    return res;
}
