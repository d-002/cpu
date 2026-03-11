#include "aliases_io.h"

#include <stdlib.h>
#include <string.h>

#include "aliases_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/numstr.h"

int handle_in_2(struct instruction *instruction, struct queue *out)
{
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    int port_value = atoi_token(instruction->args_queue->head->data);
    int register_value = atoi_token(instruction->args_queue->tail->data);

    char *port_index = my_itoa(port_value);
    char *register_index = my_itoa(register_value);
    if (port_index == NULL || register_index == NULL)
        goto err;

    struct token *port_as_value = token_create(NUMBER_DEC, port_index, 1);
    struct token *register_as_value =
        token_create(NUMBER_DEC, register_index, 1);
    free(port_index);
    free(register_index);
    struct token *r0 = token_create(REGISTER, "%r0", 3);
    struct token *r7 = token_create(REGISTER, "%r7", 3);
    struct token *r0_dup = r0 == NULL ? NULL : token_dup(r0);
    struct token *r7_dup = r7 == NULL ? NULL : token_dup(r7);

    if (port_as_value == NULL || register_as_value == NULL || r0_dup == NULL
        || r7_dup == NULL)
    {
        token_destroy(port_as_value, true);
        token_destroy(register_as_value, true);
        token_destroy(r0, true);
        token_destroy(r7, true);
        token_destroy(r0_dup, true);
        token_destroy(r7_dup, true);
        goto err;
    }

    struct instruction *i1 =
        instruction_helper(instruction->file_line, instruction->real_line,
                           "LDI", 1, register_as_value);
    struct instruction *i2 =
        instruction_helper(instruction->file_line, instruction->real_line + 1,
                           "MOVEI", 2, r0_dup, r7_dup);
    struct instruction *i3 =
        instruction_helper(instruction->file_line, instruction->real_line + 2,
                           "LDI", 1, port_as_value);
    if (i1 == NULL || i2 == NULL || i3 == NULL)
    {
        if (i1 == NULL)
            token_destroy(register_as_value, true);
        if (i3 == NULL)
            token_destroy(port_as_value, true);
        if (i2 == NULL)
        {
            token_destroy(r0_dup, true);
            token_destroy(r7_dup, true);
        }
        token_destroy(r0, true);
        token_destroy(r7, true);
        instruction_destroy(i1);
        instruction_destroy(i2);
        instruction_destroy(i3);
        goto err;
    }

    token_destroy(instruction->args_queue->head->data, true);
    token_destroy(instruction->args_queue->tail->data, true);
    instruction->args_queue->head->data = r0;
    instruction->args_queue->tail->data = r7;
    instruction->real_line += 3;

    if (queue_enqueue(out, i1) || queue_enqueue(out, i2)
        || queue_enqueue(out, i3) || queue_enqueue(out, instruction))
    {
        instruction_destroy(i1);
        instruction_destroy(i2);
        instruction_destroy(i3);
        instruction_destroy(instruction);
        goto err;
    }

    return SUCCESS;

err:
    log_alloc_error(instruction->file_line);
    return ALLOC_ERROR;
}

int handle_out_2(struct instruction *instruction, struct queue *out)
{
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    int port_value = atoi_token(instruction->args_queue->tail->data);
    int register_value = atoi_token(instruction->args_queue->head->data);
    if (register_value == 0)
        logwarn(instruction->file_line,
                "Used %%r0 as source operand in OUT will be overwritten by "
                "alias expansion");

    char *port_index = my_itoa(port_value);
    if (port_index == NULL)
        goto err;

    struct token *port_as_value = token_create(NUMBER_DEC, port_index, 1);
    free(port_index);
    struct token *r0 = token_create(REGISTER, "%r0", 3);

    if (port_as_value == NULL || r0 == NULL)
    {
        token_destroy(port_as_value, true);
        token_destroy(r0, true);
        goto err;
    }

    struct instruction *i =
        instruction_helper(instruction->file_line, instruction->real_line,
                           "LDI", 1, port_as_value);
    if (i == NULL)
    {
        token_destroy(r0, true);
        instruction_destroy(i);
        goto err;
    }

    token_destroy(instruction->args_queue->tail->data, true);
    instruction->args_queue->tail->data = r0;
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
