#include "aliases_io.h"

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

    // assuming there are no more than 10 IO ports for simplicity for now
    int value = atoi_token(instruction->args_queue->head->data);
    char index = (value % 10) + '0';
    struct token *port_as_value = token_create(NUMBER_DEC, &index, 1);
    if (port_as_value == NULL)
        goto err;

    struct instruction *i =
        instruction_helper(instruction->file_line, instruction->real_line,
                           "LDI", 1, port_as_value);
    if (i == NULL)
    {
        token_destroy(port_as_value, true);
        goto err;
    }

    struct token *r0 = token_create(REGISTER, "%r0", 3);
    if (r0 == NULL)
    {
        instruction_destroy(i);
        goto err;
    }

    token_destroy(instruction->args_queue->head->data, true);
    instruction->args_queue->head->data = r0;

    if (queue_enqueue(out, i) || queue_enqueue(out, instruction))
    {
        instruction_destroy(i);
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

    // assuming there are no more than 10 IO ports for simplicity for now
    int value = atoi_token(instruction->args_queue->tail->data);
    char index = (value % 10) + '0';
    struct token *port_as_value = token_create(NUMBER_DEC, &index, 1);
    if (port_as_value == NULL)
        goto err;

    struct instruction *i =
        instruction_helper(instruction->file_line, instruction->real_line,
                           "LDI", 1, port_as_value);
    if (i == NULL)
    {
        token_destroy(port_as_value, true);
        goto err;
    }

    struct token *r0 = token_create(REGISTER, "%r0", 3);
    if (r0 == NULL)
    {
        instruction_destroy(i);
        goto err;
    }

    token_destroy(instruction->args_queue->tail->data, true);
    instruction->args_queue->tail->data = r0;

    if (queue_enqueue(out, i) || queue_enqueue(out, instruction))
    {
        instruction_destroy(i);
        goto err;
    }

    return SUCCESS;

err:
    log_alloc_error(instruction->file_line);
    return ALLOC_ERROR;
}
