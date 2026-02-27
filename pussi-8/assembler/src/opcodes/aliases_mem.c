#include "aliases_mem.h"

#include <string.h>

#include "aliases_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"

int handle_ldi_2(struct instruction *instruction, struct queue *out)
{
    // ldi data; movei %r0,destination
    if (instruction->args_queue->length != 2)
    {
        logerror(instruction->file_line, "Expected 2 arguments, got %ld",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *data = queue_dequeue(instruction->args_queue);

    struct instruction *i1 = instruction_helper(
        instruction->file_line, instruction->real_line, "LDI", 1, data);
    if (i1 == NULL)
    {
        token_destroy(data, 1);
        log_alloc_error(instruction->file_line);
        return ALLOC_ERROR;
    }

    char r0_s[] = "%r0";
    struct token *r0 = token_create(REGISTER, r0_s, strlen(r0_s));
    if (r0 == NULL)
    {
        log_alloc_error(instruction->file_line);
        return ALLOC_ERROR;
    }
    struct token *destination = queue_dequeue(instruction->args_queue);

    struct instruction *i2 =
        instruction_helper(instruction->file_line, instruction->real_line,
                           "MOVEI", 2, r0, destination);

    if (i2 == NULL)
    {
        instruction_destroy(i1);
        token_destroy(r0, 1);
        token_destroy(destination, 1);
        log_alloc_error(instruction->file_line);
        return ALLOC_ERROR;
    }

    if (queue_enqueue(out, i1) || queue_enqueue(out, i2))
    {
        instruction_destroy(i1);
        instruction_destroy(i2);
        log_alloc_error(instruction->file_line);
        return ALLOC_ERROR;
    }

    instruction_destroy(instruction);
    return SUCCESS;
}
