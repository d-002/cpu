#ifndef OPCODES_H
#define OPCODES_H

#include "parser/state.h"

enum opcodes
{
    // hardware opcodes
    NOP = 0,
    COND = 1,
    JUMPI = 2,
    JUMP = 3,
    JUMPR = 4,
    LDI = 5,
    MOVEI = 6,
    MOVEA = 7,
    RTC = 8,
    CTR = 9,
    TIMER = 10,
    MUL = 11,
    DIV = 12,
    MOD = 13,
    IN = 14,
    OUT = 15,
    OR = 16,
    NOR = 17,
    ADD = 18,
    SUB = 19,
    XOR = 20,
    XNOR = 21,
    AND = 22,
    NAND = 23,
    NOT = 24,
    LSH = 25,
    RSH = 26,
    ROR = 27,
    ROL = 28,
    PUSH = 29,
    POP = 30,
    HALT = 31,

    // qol aliases
    LDI_2,
    OR_3,
    NOR_3,
    ADD_3,
    SUB_3,
    XOR_3,
    XNOR_3,
    AND_3,
    NAND_3,
    NOT_3,
    LSH_3,
    RSH_3,
    ROR_3,
    ROL_3,
    MUL_3,
    DIV_3,
    MOD_3,

    // jump aliases
    JEQ,
    JNE,
    JCS,
    JCC,
    JPL,
    JMI,
    JVS,
    JVC,
    JOS,
    JOC,
    JUS,
    JUC,
    JMP,

    // other
    UNKNOWN_OPCODE,
};

enum opcodes get_opcode(struct instruction *instruction);

#endif /* ! OPCODES_H */
