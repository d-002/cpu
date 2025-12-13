import specs from "./specs.js"

const names = {
    0: "NOP",
    1: "COND",
    2: "JUMPI",
    3: "JUMP",
    4: "JUMPR",
    5: "LDI",
    6: "MOVEI",
    7: "MOVEA",
    8: "RTC",
    9: "CTR",
    10: "TIMER",
    11: "MUL",
    12: "DIV",
    13: "MOD",
    14: "IN",
    15: "OUT",
    16: "OR",
    17: "NOR",
    18: "ADD",
    19: "SUB",
    20: "XOR",
    21: "XNOR",
    22: "AND",
    23: "NAND",
    24: "NOT",
    25: "LSH",
    26: "RSH",
    27: "ROR",
    28: "ROL",
    29: "PUSH",
    30: "POP",
    31: "HLT",
};

const funcs = {
    NOP: (_, __, ___, ____) => "NOP -- do nothing",
    COND: (_, v, ___, ____) => "COND -- SR & " + v + " -> CB",
    JUMPI: (_, v, ___, ____) => "JUMPI -- " + v,
    JUMP: (_, __, a, b) => "JUMP -- (r" + a + ") (r" + b + ")",
    JUMPR: (_, __, a, b) => "JUMPR -- (r" + a + ") (r" + b + ")",
    LDI: (_, v, ___, ____) => "LDI -- " + v + " -> r0",
    MOVEI: (_, __, d, s) => "MOVEI -- " + "r" + s + " -> r" + d,
    MOVEA: (_, __, d, s) => "MOVEA -- " + "r(r" + s + ") -> r(r" + d + ")",
    RTC: (_, __, d, s) => "RTC -- r" + s + " -> M(r" + d + ")",
    CTR: (_, __, s, d) => "CTR -- M(r" + s + ") -> r" + d,
    TIMER: (_, __, d, b) => "TIMER -- timer >> " + 8*b + " -> r" + d,
    MUL: (_, __, b, a) => "MUL -- r" + a + " * r" + b + " -> r0",
    DIV: (_, __, b, a) => "DIV -- r" + a + " / r" + b + " -> r0",
    MOD: (_, __, b, a) => "MOD -- r" + a + " % r" + b + " -> r0",
    IN: (_, __, d, s) => "IN -- r" + s + " -> E(r" + d + ")",
    OUT: (_, __, s, d) => "OUT -- R(r" + s + " -> r" + d,
    OR: (_, __, a, b) => "OR -- r" + a + " | " + b + " -> r0, set SR",
    NOR: (_, __, a, b) => "NOR -- !(r" + a + " | " + b + ") -> r0, set SR",
    ADD: (_, __, a, b) => "ADD -- r" + a + " + " + b + " -> r0, set SR",
    SUB: (_, __, a, b) => "SUB -- r" + a + " - " + b + " -> r0, set SR",
    XOR: (_, __, a, b) => "XOR -- r" + a + " ^ " + b + " -> r0, set SR",
    XNOR: (_, __, a, b) => "XNOR -- !(r" + a + " ^ " + b + ") -> r0, set SR",
    AND: (_, __, a, b) => "AND -- r" + a + " & " + b + " -> r0, set SR",
    NAND: (_, __, a, b) => "NAND -- !(r" + a + " & " + b + ") -> r0, set SR",
    NOT: (_, __, a, ___) => "NOT -- !r" + a + " -> r0, set SR",
    LSH: (_, __, a, ___) => "LSH -- r" + a + " << 1 -> r0, set SR",
    RSH: (_, __, a, ___) => "RSH -- r" + a + " >> 1 -> r0, set SR",
    ROR: (_, __, a, ___) => "ROR -- (r" + a + " >> 1) + (r" + a + " & 1 << 7) -> r0, set SR",
    ROL: (_, __, a, ___) => "ROL -- (r" + a + " << 1) + (r" + a + "& 0x80 >> 7) -> r0, set SR",
    PUSH: (_, __, ___, ____) => "PUSH -- PC+1 -> ST++, set SR",
    POP: (_, __, ___, ____) => "POP -- -ST -> PC, set SR",
    HLT: (_, __, ___, ____) => "HLT -- stop program execution",
}

export function getInstructionName(opcode) {
    return names[opcode % specs.numInstructions];
}

export function formatInstruction(value) {
    const opcode = value >> specs.instructionSize / 2;
    const immediate = value & ((1 << specs.instructionSize / 2) - 1);
    const argsHi = immediate >> (specs.instructionSize / 4);
    const argsLo = immediate & ((1 << (specs.instructionSize / 4)) - 1);

    const name = getInstructionName(opcode);
    const func = funcs[name];

    return func(opcode, immediate, argsHi, argsLo);
}
