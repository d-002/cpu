#include "to_machine_utils.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/numstr.h"

#define JUMP_NO_SPECIAL 255

int expect_n_args(struct instruction *instruction, size_t count, int values[],
                  enum token_type types[], bool silent)
{
    if (instruction->args_queue->length != count)
    {
        if (!silent)
            logerror(instruction->file_line,
                     "Expected %ld arguments for %s, got %ld.", count,
                     instruction->opcode->data,
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
            if (!silent)
                logerror(instruction->file_line,
                         "Expected %s but got %s as argument number %d for %s.",
                         type2name(types[i]), type2name(token->type), i + 1,
                         instruction->opcode->data);
            return INSTRUCTION_ERROR;
        }

        int n = atoi_token(token);
        values[i++] = n;
    }

    return SUCCESS;
}

int add_to_content(int file_line, struct queue *content, short n)
{
    short *data = malloc(sizeof(short));
    if (data == NULL)
    {
        log_alloc_error(file_line);
        return ALLOC_ERROR;
    }

    *data = n;

    if (queue_enqueue(content, data))
    {
        free(data);
        log_alloc_error(file_line);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}

int add_nop(int file_line, struct queue *content, int n)
{
    for (int i = 0; i < n; i++)
    {
        int res = add_to_content(file_line, content, (NOP << 8));
        if (res != SUCCESS)
            return res;
    }

    return SUCCESS;
}

int handle_jump_special(int file_line, struct queue *content, int arg,
                        enum token_type type)
{
    // if the JUMP address is a single register, write a 0 to %r0 and use it as
    // high address
    if (type == REGISTER)
    {
        int res = add_to_content(file_line, content, (LDI << 8));
        if (res != SUCCESS)
            return res;
        res = add_to_content(file_line + 1, content, (JUMP << 8) + (arg & 255));
        if (res != SUCCESS)
            return res;
        return add_nop(file_line + 2, content, 1);
    }

    // if the target address is small enough, use a JUMPI instruction
    loginfo(0, "%d", arg);
    if (arg < 256)
    {
        int res = add_to_content(file_line, content, (JUMPI << 8) + arg);
        if (res != SUCCESS)
            return res;
        return add_nop(file_line + 1, content, 2);
    }

    // if the relative target address is small enough, use a JUMPR instruction
    int relative = arg - file_line;
    if (relative >= 0 && relative < 256)
    {
        int res = add_to_content(file_line, content, (JUMPR << 8) + relative);
        if (res != SUCCESS)
            return res;
        return add_nop(file_line + 1, content, 2);
    }

    // normal instruction
    return JUMP_NO_SPECIAL;
}

int to_machine_i_jumps(struct instruction *instruction, enum opcodes opcode,
                       struct queue *content)
{
    int res = SUCCESS;
    int args[2];
    enum token_type types[2] = { REGISTER, REGISTER };

    switch (opcode)
    {
    case COND:
    case JUMPI:
        types[0] = NUMBER_PSEUDOTYPE;
        res = expect_n_args(instruction, 1, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    case JUMPR:
        types[0] = NUMBER_PSEUDOTYPE;
        res = expect_n_args(instruction, 1, args, types, false);
        if (res == SUCCESS)
            break;
        break;
    case JUMP:
        // try a "normal" jump with two register arguments
        res = expect_n_args(instruction, 2, args, types, true);
        if (res == SUCCESS)
            break;

        // if that fails, try the single argument one with a register
        res = expect_n_args(instruction, 1, args, types, true);
        if (res == SUCCESS)
            return handle_jump_special(instruction->file_line, content, args[0],
                                       types[0]);

        // if that also fails, try the single argument one with a number
        types[0] = NUMBER_PSEUDOTYPE;
        res = expect_n_args(instruction, 1, args, types, false);
        if (res != SUCCESS)
            return res;
        res = handle_jump_special(instruction->file_line, content, args[0],
                                  types[0]);
        if (res == JUMP_NO_SPECIAL)
            break;
        return res;
    default:
        return res;
    }

    switch (opcode)
    {
    case COND:
        return add_to_content(instruction->file_line, content,
                              (COND << 8) + (args[0] & 255));
    case JUMPI:
        return add_to_content(instruction->file_line, content,
                              (JUMPI << 8) + (args[0] & 255));
    case JUMPR:
        return add_to_content(instruction->file_line, content,
                              (JUMPR << 8) + (args[0] & 255));
    case JUMP:
        res = add_to_content(instruction->file_line, content,
                             (JUMP << 8) + ((args[0] & 15) << 4)
                                 + (args[1] & 15));
        if (res != SUCCESS)
            return res;
        return add_nop(instruction->file_line + 1, content, 2);
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
        res = expect_n_args(instruction, 1, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    case MOVEI:
    case MOVEA:
    case RTC:
    case CTR:
    case TIMER:
        res = expect_n_args(instruction, 2, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    default:
        return res;
    }

    switch (opcode)
    {
    case LDI:
        return add_to_content(instruction->file_line, content,
                              (LDI << 8) + (args[0] & 255));
    case MOVEI:
    case MOVEA:
    case RTC:
    case TIMER:
        return add_to_content(instruction->file_line, content,
                              (opcode << 8) + ((args[1] & 15) << 4)
                                  + (args[0] & 15));
    default: // case CTR
        return add_to_content(instruction->file_line, content,
                              (CTR << 8) + ((args[0] & 15) << 4)
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
        res = expect_n_args(instruction, 1, args, types, false);
        if (res != SUCCESS)
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
        res = expect_n_args(instruction, 2, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    default:
        return res;
    }

    return add_to_content(instruction->file_line, content,
                          (opcode << 8) + ((args[0] & 15) << 4)
                              + (args[1] & 15));
}

int to_machine_i_misc(struct instruction *instruction, enum opcodes opcode,
                      struct queue *content)
{
    int res = SUCCESS;
    int args[2];
    enum token_type types[2] = { REGISTER, REGISTER };

    switch (opcode)
    {
    case IN:
        res = expect_n_args(instruction, 2, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    case OUT:
        res = expect_n_args(instruction, 2, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    case PUSH:
    case POP:
    case HALT:
    case NOP:
        res = expect_n_args(instruction, 0, args, types, false);
        if (res != SUCCESS)
            return res;
        break;
    default:
        return res;
    }

    switch (opcode)
    {
    case IN:
        return add_to_content(instruction->file_line, content,
                              (IN << 8) + ((args[1] & 15) << 4)
                                  + (args[0] & 15));
        break;
    case OUT:
        return add_to_content(instruction->file_line, content,
                              (OUT << 8) + ((args[0] & 15) << 4)
                                  + (args[1] & 15));
        break;
    default:
        return add_to_content(instruction->file_line, content, opcode << 8);
    }

    return SUCCESS;
}
