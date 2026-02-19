#include "to_machine.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/numstr.h"

int expect_n_args(struct instruction *instruction, size_t count, int values[],
                  enum token_type types[])
{
    if (instruction->args_queue->length != count)
    {
        logerror(instruction->line, "Expected %ld arguments for %s, got %ld",
                 count, instruction->opcode->data,
                 instruction->args_queue->length);
        return INSTRUCTION_ERROR;
    }

    size_t i = 0;
    for (struct token *token = queue_iter_start(instruction->args_queue); token;
         token = queue_iter_next(instruction->args_queue))
    {
        int ok = token->type == types[i];
        if (!ok && types[i] == NUMBER_PSEUDOTYPE)
            ok = token->type == NUMBER_DEC || token->type == NUMBER_BIN
                || token->type == NUMBER_HEX;

        if (!ok)
        {
            logerror(instruction->line,
                     "Expected %s but got %s as argument for %s",
                     type2name(types[i]), type2name(token->type),
                     instruction->args_queue->length);
            return INSTRUCTION_ERROR;
        }

        int n = atoi_token(token);
        values[i++] = n;
    }

    return SUCCESS;
}

int add_to_content(int line, struct queue *content, short n)
{
    short *data = malloc(sizeof(short));
    if (data == NULL)
    {
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    *data = n;

    if (queue_enqueue(content, data))
    {
        free(data);
        log_alloc_error(line);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}

int to_machine_i_jumps(struct instruction *instruction, enum opcodes opcode,
                       struct queue *content)
{
    int res = SUCCESS;
    int args[1];
    enum token_type types[1] = { NUMBER_PSEUDOTYPE };

    switch (opcode)
    {
    case COND:
    case JUMPI:
        res = expect_n_args(instruction, 1, args, types);
        if (res)
            return res;
        break;
    case JUMP:
    case JUMPR:
        return res;
    default:
        return res;
    }

    switch (opcode)
    {
    case COND:
        return add_to_content(instruction->line, content,
                              (COND << 8) + (args[0] & 255));
    case JUMPI:
        return add_to_content(instruction->line, content,
                              (JUMPI << 8) + (args[0] & 255));
    case JUMP:
        break;
    default:
        break;
    }

    return res;
}

int to_machine_i_data(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    int res = SUCCESS;
    int args[2];
    enum token_type types[2] = { REGISTER, REGISTER };

    switch (opcode)
    {
    case LDI:
        types[0] = NUMBER_PSEUDOTYPE;
        res = expect_n_args(instruction, 1, args, types);
        if (res)
            return res;
        break;
    case MOVEI:
    case MOVEA:
    case RTC:
    case CTR:
    case TIMER:
        res = expect_n_args(instruction, 2, args, types);
        if (res)
            return res;
        break;
    default:
        return res;
    }

    switch (opcode)
    {
    case LDI:
        return add_to_content(instruction->line, content,
                              (LDI << 8) + (args[0] & 255));
    case MOVEI:
    case MOVEA:
    case RTC:
    case TIMER:
        return add_to_content(instruction->line, content,
                              (opcode << 8) + (args[1] << 4 & 15)
                                  + (args[0] & 15));
    default:
        return add_to_content(instruction->line, content,
                              (CTR << 8) + (args[0] << 4 & 15)
                                  + (args[1] & 15));
    }

    return res;
}

int to_machine_i_calc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    int res = SUCCESS;
    int args[2];
    enum token_type types[2] = { REGISTER, REGISTER };

    switch (opcode)
    {
    case NOT:
    case LSH:
    case RSH:
    case ROR:
    case ROL:
        res = expect_n_args(instruction, 1, args, types);
        if (res)
            return res;
        break;
    case MUL:
    case DIV:
    case MOD:
    case OR:
    case NOR:
    case ADD:
    case SUB:
    case XOR:
    case XNOR:
    case AND:
    case NAND:
        res = expect_n_args(instruction, 2, args, types);
        if (res)
            return res;
        break;
    default:
        return res;
    }

    return add_to_content(instruction->line, content,
                          (opcode << 8) + (args[0] << 4 & 15) + (args[1] & 15));
}

int to_machine_i_misc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    int res = SUCCESS;
    int args[2];
    enum token_type types[2];

    switch (opcode)
    {
    case IN:
        types[0] = PORT;
        types[1] = REGISTER;
        res = expect_n_args(instruction, 2, args, types);
        if (res)
            return res;
        break;
    case OUT:
        types[0] = REGISTER;
        types[1] = PORT;
        res = expect_n_args(instruction, 2, args, types);
        if (res)
            return res;
        break;
    case PUSH:
    case POP:
    case HALT:
    case NOP:
        res = expect_n_args(instruction, 0, args, types);
        if (res)
            return res;
        break;
    default:
        return res;
    }

    switch (opcode)
    {
    case IN:
        return add_to_content(instruction->line, content,
                              (args[0] << 4 & 15) + (args[1] & 15));
        break;
    case OUT:
        return add_to_content(instruction->line, content,
                              (args[1] << 4 & 15) + (args[0] & 15));
        break;
    default:
        return add_to_content(instruction->line, content, 0);
    }

    return SUCCESS;
}
