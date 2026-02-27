#include "instruction.h"

#include <ctype.h>
#include <stdlib.h>

#include "queue/queue.h"

struct instruction *instruction_create(int line, int real_line,
                                       struct token *opcode)
{
    struct instruction *instruction = malloc(sizeof(struct instruction));
    if (instruction == NULL)
        return NULL;

    instruction->opcode = opcode;
    instruction->args_queue = queue_create();
    instruction->line = line;
    instruction->real_line = real_line;

    if (instruction->args_queue == NULL)
    {
        free(instruction);
        return NULL;
    }

    // make the opcode uppercase for comparisons later
    for (size_t i = 0; i < opcode->length; i++)
        opcode->data[i] = toupper(opcode->data[i]);

    return instruction;
}

void instruction_destroy(struct instruction *instruction)
{
    token_destroy(instruction->opcode, 1);

    while (instruction->args_queue->length)
        token_destroy(queue_dequeue(instruction->args_queue), 1);
    queue_destroy(instruction->args_queue);

    free(instruction);
}
