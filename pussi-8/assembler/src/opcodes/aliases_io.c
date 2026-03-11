#include "aliases_io.h"

#include <string.h>

#include "aliases_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/numstr.h"

static int helper(struct instruction *instruction, struct queue *out,
                  struct token **port_arg, struct token **register_arg)
{
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld.",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    // assuming there are no more than 10 IO ports for simplicity for now
    int value = atoi_token(*port_arg);
    char index = (value % 10) + '0';
    struct token *port_as_value = token_create(NUMBER_DEC, &index, 1);
    value = atoi_token(*register_arg);
    index = (value % 10) + '0';
    struct token *register_as_value = token_create(NUMBER_DEC, &index, 1);
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
        token_destroy(port_as_value, true);
        token_destroy(r0, true);
        token_destroy(r7, true);
        token_destroy(r0_dup, true);
        token_destroy(r7_dup, true);
        instruction_destroy(i1);
        instruction_destroy(i2);
        instruction_destroy(i3);
        goto err;
    }

    token_destroy(*port_arg, true);
    token_destroy(*register_arg, true);
    *port_arg = r0;
    *register_arg = r7;
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

int handle_in_2(struct instruction *instruction, struct queue *out)
{
    return helper(instruction, out,
                  (struct token **)(&instruction->args_queue->head->data),
                  (struct token **)(&instruction->args_queue->tail->data));
}

int handle_out_2(struct instruction *instruction, struct queue *out)
{
    return helper(instruction, out,
                  (struct token **)(&instruction->args_queue->tail->data),
                  (struct token **)(&instruction->args_queue->head->data));
}
