#include "to_machine.h"

#include <stdio.h>
#include <stdlib.h>

#include "logger/logger.h"
#include "opcodes/aliases_alu.h"
#include "opcodes/aliases_jmp.h"
#include "opcodes/aliases_mem.h"
#include "opcodes/opcodes.h"
#include "to_machine_utils.h"
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
        logerror(NO_LINE, "Unknown opcode: '%s'.", instruction->opcode->data);
        instruction_destroy(instruction);
        return INSTRUCTION_ERROR;
    }

    int res;
    switch (opcode)
    {
    case MOV:
        res = handle_mov(instruction, queue);
        break;
    case TEST:
        res = handle_test(instruction, queue);
        break;
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
        res = handle_custom_jump(instruction, queue, "16");
        break;
    case JVC:
        res = handle_custom_jump(instruction, queue, "17");
        break;
    case JOS:
        res = handle_custom_jump(instruction, queue, "8");
        break;
    case JOC:
        res = handle_custom_jump(instruction, queue, "9");
        break;
    case JUS:
        res = handle_custom_jump(instruction, queue, "4");
        break;
    case JUC:
        res = handle_custom_jump(instruction, queue, "5");
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

char *make_description(struct instruction *instruction)
{
    // WARNING: this is not portable, mind the constant padding size
    size_t size = 6 + 1;
    for (struct token *arg = queue_iter_start(instruction->args_queue); arg;
         arg = queue_iter_next(instruction->args_queue))
        size += arg->length + 1;

    char *copy = calloc(size, sizeof(char));
    size_t index = 0;
    sprintf(copy + index, "%6s", instruction->opcode->data);
    index += 6;

    int first = 1;
    for (struct token *arg = queue_iter_start(instruction->args_queue); arg;
         arg = queue_iter_next(instruction->args_queue))
    {
        copy[index++] = first ? ' ' : ',';
        first = 0;
        sprintf(copy + index, "%s", arg->data);
        index += arg->length;
    }

    return copy;
}

void print_instruction(int file_line, int real_line, char *description,
                       struct queue *group)
{
    int several = group->length > 1 ? group->length : 0;
    int index = 0;
    for (short *binary_encoded = queue_iter_start(group); binary_encoded;
         binary_encoded = queue_iter_next(group), index++)
    {
        int opcode = *binary_encoded >> 8;
        char opcode_b[9];
        char args_b[9];
        fill_buf_with_bin(opcode, opcode_b, 8);
        fill_buf_with_bin(*binary_encoded & 255, args_b, 8);

        printf(" 0x%08X | %s %s |", real_line + index, opcode_b, args_b);
        if (several)
        {
            if (index == several - 1)
                printf(" %d/%d  |", several, several);
            else
                printf(" %d    |", index + 1);
        }
        else
            printf(" -    |");

        if (index == 0)
            printf(" %-4d | %s\n", file_line, description ? description : "?");
        else
            puts("      |");
    }
}

int to_machine_code(struct cli_args *args, struct state *state,
                    struct queue *queue, struct queue *temp_content,
                    struct queue *content)
{
    if (args->print)
    {
        printf(" %s:\n", state->file_name);
        puts(" addr       | opcode   args     | part | line | opcode args");
    }

    int prev_file_line = -1;
    while (state->instructions->length)
    {
        // make a copy of the raw data for prettyprinting
        struct instruction *raw = queue_head(state->instructions);
        int file_line = raw->file_line;
        int real_line = raw->real_line;
        char *description = NULL;
        if (args->print && file_line != prev_file_line)
        {
            description = make_description(raw);
            prev_file_line = file_line;
        }

        int res = gen_next_instruction(state, queue);
        if (res != SUCCESS)
        {
            free(description);
            return res;
        }

        while (queue->length)
        {
            struct instruction *instruction = queue_dequeue(queue);

            enum opcodes opcode = get_opcode(instruction);
            res = to_machine_i_jumps(instruction, opcode, temp_content);
            res |= to_machine_i_data(instruction, opcode, temp_content);
            res |= to_machine_i_calc(instruction, opcode, temp_content);
            res |= to_machine_i_misc(instruction, opcode, temp_content);

            instruction_destroy(instruction);

            if (res != SUCCESS)
            {
                free(description);
                return res;
            }
        }

        if (args->print)
        {
            print_instruction(file_line, real_line, description, temp_content);
            free(description);
        }

        while (temp_content->length)
        {
            res = queue_enqueue(content, queue_dequeue(temp_content));
            if (res != SUCCESS)
                return res;
        }
    }

    return SUCCESS;
}
