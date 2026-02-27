#include "aliases_jmp.h"

#include <string.h>

#include "aliases_utils.h"
#include "logger/logger.h"
#include "utils/errors.h"

int handle_custom_jump(struct instruction *instruction, struct queue *out,
                       char *cond_code)
{
    if (instruction->args_queue->length != 1)
    {
        logerror(instruction->line, "Expected 1 argument, got %ld",
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    struct token *cond = token_create(NUMBER_DEC, cond_code, strlen(cond_code));
    if (cond == NULL)
    {
        log_alloc_error(instruction->line);
        return ALLOC_ERROR;
    }

    struct instruction *i1 = instruction_helper(
        instruction->line, instruction->real_line, "COND", 1, cond);
    if (i1 == NULL)
    {
        token_destroy(cond, 1);
        log_alloc_error(instruction->line);
        return ALLOC_ERROR;
    }

    struct token *addr = queue_dequeue(instruction->args_queue);
    struct instruction *i2 = instruction_helper(
        instruction->line, instruction->real_line + 1, "JUMP", 1, addr);

    if (i2 == NULL)
    {
        instruction_destroy(i1);
        token_destroy(addr, 1);
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
