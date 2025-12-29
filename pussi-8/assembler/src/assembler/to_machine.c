#include "to_machine.h"

#include <stdlib.h>

#include "err.h"
#include "logger.h"
#include "numstr.h"

int expect_n_args(struct instruction *instruction, size_t count)
{
    if (instruction->args_queue->length == count)
        return SUCCESS;

    logerror(instruction->line, "Expected %ld arguments, got %ld",
             instruction->args_queue->length);
    return INSTRUCTION_ERROR;
}

int to_machine_i_jumps(struct instruction *instruction, enum opcodes opcode,
                       struct queue *content)
{
    int res;

    int arg1 =
        atoi_base(instruction->line, queue_iter_start(instruction->args_queue));
    struct token *token = queue_iter_next(instruction->args_queue);
    int arg2 = token ? atoi_base(instruction->line, token) : 0;

    content++;
    switch (opcode)
    {
    case COND:
        res = expect_n_args(instruction, 1);
        if (res)
            return res;
        return (COND << 8) + (arg1 & 255);
        break;
    case JUMPI:
        res = expect_n_args(instruction, 2);
        if (res)
            return res;
        return (JUMPI << 8) + (arg1 & 255);
        break;
    case JUMP:
        break;
    case JUMPR:
        break;
    default:
        break;
    }

    return SUCCESS;
}

int to_machine_i_data(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    instruction++;
    content++;
    switch (opcode)
    {
    case LDI:
        break;
    case MOVEI:
        break;
    case MOVEA:
        break;
    case RTC:
        break;
    case CTR:
        break;
    case TIMER:
        break;
    default:
        break;
    }

    return SUCCESS;
}

int to_machine_i_calc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    instruction++;
    content++;
    switch (opcode)
    {
    case MUL:
        break;
    case DIV:
        break;
    case MOD:
        break;
    case OR:
        break;
    case NOR:
        break;
    case ADD:
        break;
    case SUB:
        break;
    case XOR:
        break;
    case XNOR:
        break;
    case AND:
        break;
    case NAND:
        break;
    case NOT:
        break;
    case LSH:
        break;
    case RSH:
        break;
    case ROR:
        break;
    case ROL:
        break;
    default:
        break;
    }

    return SUCCESS;
}

int to_machine_i_misc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    instruction++;
    content++;
    switch (opcode)
    {
    case PUSH:
        break;
    case POP:
        break;
    case HALT:
        break;
    case NOP:
        break;
    default:
        break;
    }

    return SUCCESS;
}
