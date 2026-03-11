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
        logerror(instruction->file_line, "Expected 3 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *a = queue_dequeue(instruction->args_queue);
    struct token *b = queue_dequeue(instruction->args_queue);

    struct instruction *i1 = instruction_helper(
        instruction->file_line, instruction->real_line, opcode, 2, a, b);
    if (i1 == NULL)
    {
        token_destroy(a, true);
        token_destroy(b, true);
        goto err;
    }

    char r0_s[] = "%r0";
    struct token *r0 = token_create(REGISTER, r0_s, strlen(r0_s));
    if (r0 == NULL)
        goto err;
    struct token *y = queue_dequeue(instruction->args_queue);

    struct instruction *i2 = instruction_helper(
        instruction->file_line, instruction->real_line + 1, "MOVEI", 2, r0, y);
    if (i2 == NULL)
    {
        instruction_destroy(i1);
        token_destroy(r0, true);
        token_destroy(y, true);
        goto err;
    }

    if (queue_enqueue(out, i1) || queue_enqueue(out, i2))
    {
        instruction_destroy(i1);
        instruction_destroy(i2);
        goto err;
    }

    instruction_destroy(instruction);
    return SUCCESS;

err:
    log_alloc_error(instruction->file_line);
    return ALLOC_ERROR;
}

int handle_test(struct instruction *instruction, struct queue *out)
{
    if (instruction->args_queue->length != 1)
    {
        logerror(instruction->file_line, "Expected 1 argument, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *opcode = token_create(OPCODE, "OR", 2);
    if (opcode == NULL)
        goto err;
    token_destroy(instruction->opcode, true);
    instruction->opcode = opcode;
    instruction->real_line += 1;

    struct token *copy = token_dup(instruction->args_queue->head->data);
    if (copy == NULL)
        goto err;
    if (queue_enqueue(instruction->args_queue, copy))
    {
        token_destroy(copy, true);
        goto err;
    }

    if (queue_enqueue(out, instruction))
    {
        token_destroy(opcode, true);
        goto err;
    }

    return SUCCESS;

err:
    log_alloc_error(instruction->file_line);
    return ALLOC_ERROR;
}
