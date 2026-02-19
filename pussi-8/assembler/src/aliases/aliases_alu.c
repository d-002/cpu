#include "aliases_alu.h"

#include <string.h>

#include "aliases_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"

int handle_custom_calc(struct instruction *instruction, struct queue *out,
                       char *opcode)
{
    if (instruction->args_queue->length != 3)
    {
        logerror(instruction->line, "Expected 3 arguments, got %ld",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *a = queue_dequeue(instruction->args_queue);
    struct token *b = queue_dequeue(instruction->args_queue);

    struct instruction *i1 =
        instruction_helper(instruction->line, opcode, 2, a, b);
    if (i1 == NULL)
    {
        token_destroy(a, 1);
        token_destroy(b, 1);
        log_alloc_error(instruction->line);
        return ALLOC_ERROR;
    }

    char r0_s[] = "%r0";
    struct token *r0 = token_create(REGISTER, r0_s, strlen(r0_s));
    if (r0 == NULL)
    {
        log_alloc_error(instruction->line);
        return ALLOC_ERROR;
    }
    struct token *y = queue_dequeue(instruction->args_queue);

    struct instruction *i2 =
        instruction_helper(instruction->line, "MOVEI", 2, r0, y);
    if (i2 == NULL)
    {
        instruction_destroy(i1);
        token_destroy(r0, 1);
        token_destroy(y, 1);
        log_alloc_error(instruction->line);
        return ALLOC_ERROR;
    }

    if (queue_enqueue(out, i1) || queue_enqueue(out, i2))
    {
        instruction_destroy(i1);
        instruction_destroy(i2);
        log_alloc_error(instruction->line);
        return ALLOC_ERROR;
    }

    instruction_destroy(instruction);
    return SUCCESS;
}
