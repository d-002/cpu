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
