#include "aliases_mem.h"

#include <stdlib.h>
#include <string.h>

#include "aliases_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/numstr.h"

int handle_mov(struct instruction *instruction, struct queue *out)
{
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *opcode = token_create(OPCODE, "MOVEI", 5);
    if (opcode == NULL)
        goto err;
    token_destroy(instruction->opcode, true);
    instruction->opcode = opcode;

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

int handle_ldi_2(struct instruction *instruction, struct queue *out)
{
    // ldi data; movei %r0,destination
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *data = queue_dequeue(instruction->args_queue);

    struct instruction *i1 = instruction_helper(
        instruction->file_line, instruction->real_line, "LDI", 1, data);
    if (i1 == NULL)
    {
        token_destroy(data, true);
        goto err;
    }

    char r0_s[] = "%r0";
    struct token *r0 = token_create(REGISTER, r0_s, strlen(r0_s));
    if (r0 == NULL)
        goto err;
    struct token *destination = queue_dequeue(instruction->args_queue);

    struct instruction *i2 =
        instruction_helper(instruction->file_line, instruction->real_line + 1,
                           "MOVEI", 2, r0, destination);

    if (i2 == NULL)
    {
        instruction_destroy(i1);
        token_destroy(r0, true);
        token_destroy(destination, true);
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

int helper(struct instruction *instruction, struct queue *out,
           struct token **arg)
{
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    int value = atoi_token(*arg);
    char *index = my_itoa(value);
    if (index == NULL)
        goto err;

    struct token *mem_as_value = token_create(NUMBER_DEC, index, 1);
    free(index);
    struct token *r0 = token_create(REGISTER, "%r0", 3);
    if (mem_as_value == NULL || r0 == NULL)
    {
        token_destroy(mem_as_value, true);
        token_destroy(r0, true);
        goto err;
    }

    struct instruction *i = instruction_helper(
        instruction->file_line, instruction->real_line, "LDI", 1, mem_as_value);
    if (i == NULL)
    {
        token_destroy(mem_as_value, true);
        token_destroy(r0, true);
        instruction_destroy(i);
        goto err;
    }

    token_destroy(*arg, true);
    *arg = r0;
    instruction->real_line++;

    if (queue_enqueue(out, i) || queue_enqueue(out, instruction))
    {
        instruction_destroy(i);
        instruction_destroy(instruction);
        goto err;
    }

    return SUCCESS;

err:
    log_alloc_error(instruction->file_line);
    return ALLOC_ERROR;
}

int handle_rtc_2(struct instruction *instruction, struct queue *out)
{
    return helper(instruction, out,
                  (struct token **)(&instruction->args_queue->tail->data));
}

int handle_ctr_2(struct instruction *instruction, struct queue *out)
{
    return helper(instruction, out,
                  (struct token **)(&instruction->args_queue->head->data));
}
