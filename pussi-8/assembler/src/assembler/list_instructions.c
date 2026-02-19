#include "list_instructions.h"

#include <stdio.h>

#include "aliases/aliases_alu.h"
#include "aliases/aliases_jmp.h"
#include "aliases/aliases_mem.h"
#include "logger/logger.h"
#include "opcodes.h"
#include "to_machine.h"
#include "utils/errors.h"
#include "utils/numstr.h"

int gen_next_instruction(struct state *state, struct queue *queue)
{
    struct instruction *instruction = queue_dequeue(state->instructions);
    if (instruction == NULL)
        return SUCCESS;

    enum opcodes opcode = get_opcode(instruction);
    if (opcode == UNKNOWN_OPCODE)
    {
        logerror(NO_LINE, "Unknown opcode: '%s'", instruction->opcode->data);
        instruction_destroy(instruction);
        return INSTRUCTION_ERROR;
    }

    int res;
    switch (opcode)
    {
    case LDI_2:
        res = handle_ldi_2(instruction, queue);
        break;
    case OR_3:
        res = handle_custom_calc(instruction, queue, "OR");
        break;
    case NOR_3:
        res = handle_custom_calc(instruction, queue, "NOR");
        break;
    case ADD_3:
        res = handle_custom_calc(instruction, queue, "ADD");
        break;
    case SUB_3:
        res = handle_custom_calc(instruction, queue, "SUB");
        break;
    case XOR_3:
        res = handle_custom_calc(instruction, queue, "XOR");
        break;
    case XNOR_3:
        res = handle_custom_calc(instruction, queue, "XNOR");
        break;
    case AND_3:
        res = handle_custom_calc(instruction, queue, "AND");
        break;
    case NAND_3:
        res = handle_custom_calc(instruction, queue, "NAND");
        break;
    case NOT_3:
        res = handle_custom_calc(instruction, queue, "NOT");
        break;
    case LSH_3:
        res = handle_custom_calc(instruction, queue, "LSH");
        break;
    case RSH_3:
        res = handle_custom_calc(instruction, queue, "RSH");
        break;
    case ROR_3:
        res = handle_custom_calc(instruction, queue, "OOR");
        break;
    case ROL_3:
        res = handle_custom_calc(instruction, queue, "ROL");
        break;
    case MUL_3:
        res = handle_custom_calc(instruction, queue, "MUL");
        break;
    case DIV_3:
        res = handle_custom_calc(instruction, queue, "DIV");
        break;
    case MOD_3:
        res = handle_custom_calc(instruction, queue, "MOD");
        break;
    case JZ:
        res = handle_custom_jump(instruction, queue, "128");
        break;
    case JEQ:
        res = handle_custom_jump(instruction, queue, "128");
        break;
    case JNZ:
        res = handle_custom_jump(instruction, queue, "129");
        break;
    case JNE:
        res = handle_custom_jump(instruction, queue, "129");
        break;
    case JCS:
        res = handle_custom_jump(instruction, queue, "65");
        break;
    case JCC:
        res = handle_custom_jump(instruction, queue, "64");
        break;
    case JPL:
        res = handle_custom_jump(instruction, queue, "33");
        break;
    case JMI:
        res = handle_custom_jump(instruction, queue, "32");
        break;
    case JVS:
        res = handle_custom_jump(instruction, queue, "17");
        break;
    case JVC:
        res = handle_custom_jump(instruction, queue, "16");
        break;
    case JOS:
        res = handle_custom_jump(instruction, queue, "9");
        break;
    case JOC:
        res = handle_custom_jump(instruction, queue, "8");
        break;
    case JUS:
        res = handle_custom_jump(instruction, queue, "5");
        break;
    case JUC:
        res = handle_custom_jump(instruction, queue, "4");
        break;
    case J:
        res = handle_custom_jump(instruction, queue, "2");
        break;
    default:
        res = queue_enqueue(queue, instruction);
    }

    if (res != SUCCESS)
        instruction_destroy(instruction);

    return res;
}

void print_instruction(struct instruction *instruction, short *binary_encoded,
                       int addr)
{
    if (binary_encoded == NULL)
        printf("%08X | [didn't assemble] | %s ", addr,
               instruction->opcode->data);
    else
    {
        char opcode_b[9];
        char args_b[9];
        fill_buf_with_bin(*binary_encoded >> 8, opcode_b, 8);
        fill_buf_with_bin(*binary_encoded & 255, args_b, 8);
        printf("%08X | %s %s | %s ", addr, opcode_b, args_b,
               instruction->opcode->data);
    }

    int first = 1;
    for (struct token *arg = queue_iter_start(instruction->args_queue); arg;
         arg = queue_iter_next(instruction->args_queue))
    {
        if (!first)
            putchar(',');
        first = 0;
        printf("%s", arg->data);
    }

    putchar('\n');
}

int to_machine_code(struct cli_args *args, struct state *state,
                    struct queue *queue, struct queue *content)
{
    if (!args->run)
        puts("\naddr     | opcode   args     | description");

    while (state->instructions->length)
    {
        int res = gen_next_instruction(state, queue);
        if (res != SUCCESS)
            return res;

        while (queue->length)
        {
            struct instruction *instruction = queue_dequeue(queue);

            enum opcodes opcode = get_opcode(instruction);
            res = to_machine_i_jumps(instruction, opcode, content);
            res |= to_machine_i_data(instruction, opcode, content);
            res |= to_machine_i_calc(instruction, opcode, content);
            res |= to_machine_i_misc(instruction, opcode, content);

            if (!res && !args->run)
            {
                short *data =
                    content->tail == NULL ? NULL : content->tail->data;
                print_instruction(instruction, data, content->length - 1);
            }

            instruction_destroy(instruction);

            if (res != SUCCESS)
                return res;
        }
    }

    return SUCCESS;
}
