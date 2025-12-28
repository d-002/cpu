#include "aliases.h"

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
    case JEQ:
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
    case JMP:
        return 2;
    default:
        return 1;
    }
}
