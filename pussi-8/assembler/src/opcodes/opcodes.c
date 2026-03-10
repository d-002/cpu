#include "opcodes.h"

#include <string.h>

#include "queue/queue.h"

#define COMP(value) strcmp(instruction->opcode->data, #value) == 0
#define TEST_SIMPLE(value)                                                     \
    if (COMP(value))                                                           \
        return value;
#define TEST_ALU_3(value)                                                      \
    if (COMP(value) && arg3 != EOL)                                            \
        return value##_3;

enum opcodes get_opcode(struct instruction *instruction)
{
    struct token *arg = queue_iter_start(instruction->args_queue);
    // enum token_type arg1 = arg ? arg->type : EOL;
    arg = queue_iter_next(instruction->args_queue);
    enum token_type arg2 = arg ? arg->type : EOL;
    arg = queue_iter_next(instruction->args_queue);
    enum token_type arg3 = arg ? arg->type : EOL;

    if (COMP(LDI) && arg2)
        return LDI_2;
    TEST_SIMPLE(MOV);

    TEST_ALU_3(OR);
    TEST_ALU_3(NOR);
    TEST_ALU_3(ADD);
    TEST_ALU_3(SUB);
    TEST_ALU_3(XOR);
    TEST_ALU_3(XNOR);
    TEST_ALU_3(AND);
    TEST_ALU_3(NAND);
    TEST_ALU_3(NOT);
    TEST_ALU_3(LSH);
    TEST_ALU_3(RSH);
    TEST_ALU_3(ROR);
    TEST_ALU_3(ROL);
    TEST_ALU_3(MUL);
    TEST_ALU_3(DIV);
    TEST_ALU_3(MOD);

    TEST_SIMPLE(JZ);
    TEST_SIMPLE(JEQ);
    TEST_SIMPLE(JNZ);
    TEST_SIMPLE(JNE);
    TEST_SIMPLE(JCS);
    TEST_SIMPLE(JCC);
    TEST_SIMPLE(JPL);
    TEST_SIMPLE(JMI);
    TEST_SIMPLE(JVS);
    TEST_SIMPLE(JVC);
    TEST_SIMPLE(JOS);
    TEST_SIMPLE(JOC);
    TEST_SIMPLE(JUS);
    TEST_SIMPLE(JUC);
    TEST_SIMPLE(J);

    TEST_SIMPLE(NOP);
    TEST_SIMPLE(COND);
    TEST_SIMPLE(JUMPI);
    TEST_SIMPLE(JUMP);
    TEST_SIMPLE(JUMPR);
    TEST_SIMPLE(LDI);
    TEST_SIMPLE(MOVEI);
    TEST_SIMPLE(MOVEA);
    TEST_SIMPLE(RTC);
    TEST_SIMPLE(CTR);
    TEST_SIMPLE(TIMER);
    TEST_SIMPLE(MUL);
    TEST_SIMPLE(DIV);
    TEST_SIMPLE(MOD);
    TEST_SIMPLE(IN);
    TEST_SIMPLE(OUT);
    TEST_SIMPLE(OR);
    TEST_SIMPLE(NOR);
    TEST_SIMPLE(ADD);
    TEST_SIMPLE(SUB);
    TEST_SIMPLE(XOR);
    TEST_SIMPLE(XNOR);
    TEST_SIMPLE(AND);
    TEST_SIMPLE(NAND);
    TEST_SIMPLE(NOT);
    TEST_SIMPLE(LSH);
    TEST_SIMPLE(RSH);
    TEST_SIMPLE(ROR);
    TEST_SIMPLE(ROL);
    TEST_SIMPLE(PUSH);
    TEST_SIMPLE(POP);
    TEST_SIMPLE(HALT);

    return UNKNOWN_OPCODE;
}

int length_from_opcode(enum opcodes opcode)
{
    switch (opcode)
    {
    case LDI_2:
    case OR_3:
    case NOR_3:
    case ADD_3:
    case SUB_3:
    case XOR_3:
    case XNOR_3:
    case AND_3:
    case NAND_3:
    case NOT_3:
    case LSH_3:
    case RSH_3:
    case ROR_3:
    case ROL_3:
    case MUL_3:
    case DIV_3:
    case MOD_3:
        return 2;
    case JZ:
    case JEQ:
    case JNZ:
    case JNE:
    case JCS:
    case JCC:
    case JPL:
    case JMI:
    case JVS:
    case JVC:
    case JOS:
    case JOC:
    case JUS:
    case JUC:
    case J:
        // worst-case scenario, might take less time in practice
        // TODO: optimize this: once all instructions are build, remove
        // generated NOP instructions and shift jump values. Add to the readme
        // that this will solve most problems, but in some edge cases a
        // suboptimal choice is used.
        return 4;
    case JUMP:
        return 3;
    default:
        return 1;
    }
}
