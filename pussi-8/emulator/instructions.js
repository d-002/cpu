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
    NOP: (__, ___, ____, _____) => "NOP | do nothing",
    COND: (__, v, ____, _____) => "COND | SR & " + v + " -> CB",
    JUMPI: (__, v, ___, ____) => "JUMPI | " + v,
    JUMP: (__, ___, a, b) => "JUMP | (r" + a + ") (r" + b + ")",
    JUMPR: (__, ___, a, b) => "JUMPR | (r" + a + ") (r" + b + ")",
    LDI: (_, v, __, ___) => "LDI | " + v + " -> r0",
    MOVEI: (_, __, d, s) => "MOVEI | " + "r" + s + " -> r" + d,
    MOVEA: (_, __, d, s) => "MOVEA | " + "r(r" + s + ") -> r(r" + d + ")",
    RTC: (_, __, d, s) => "RTC | r" + s + " -> M(r" + d + ")",
    CTR: (_, __, s, d) => "CTR | M(r" + s + ") -> r" + d,
    TIMER: (__, ___, d, b) => "TIMER | timer >> " + 8*b + " -> r" + d,
    MUL: (__, ___, b, a) => "MUL | r" + a + " * r" + b + " -> r0",
    DIV: (__, ___, b, a) => "DIV | r" + a + " / r" + b + " -> r0",
    MOD: (__, ___, b, a) => "MOD | r" + a + " % r" + b + " -> r0",
    IN: (__, ___, d, s) => "IN | r" + s + " -> E(r" + d + ")",
    OUT: (__, ___, s, d) => "OUT | R(r" + s + " -> r" + d,
    OR: (__, ___, a, b) => "OR | r" + a + " | " + b + " -> r0, set SR",
    NOR: (__, ___, a, b) => "NOR | ! (r" + a + " | " + b + ") -> r0, set SR",
    ADD: (__, ___, a, b) => "ADD | r" + a + " + " + b + " -> r0, set SR",
    SUB: (__, ___, a, b) => "SUB | r" + a + " - " + b + " -> r0, set SR",
    XOR: (__, ___, a, b) => "XOR | r" + a + " ^ " + b + " -> r0, set SR",
    XNOR: (__, ___, a, b) => "XNOR | ! (r" + a + " ^ " + b + ") -> r0, set SR",
    AND: (__, ___, a, b) => "AND | r" + a + " & " + b + " -> r0, set SR",
    NAND: (__, ___, a, b) => "NAND | ! (r" + a + " & " + b + ") -> r0, set SR",
    NOT: (__, ___, a, _) => "NOT | ! r" + a + " -> r0, set SR",
    LSH: (__, ___, a, _) => "LSH | r" + a + " << 1 -> r0, set SR",
    RSH: (__, ___, a, _) => "RSH | r" + a + " >> 1 -> r0, set SR",
    ROR: (__, ___, a, _) => "ROR | (r" + a + " >> 1) + (r" + a + " & 1 << 7) -> r0, set SR",
    ROL: (__, ___, a, _) => "ROL | (r" + a + " << 1) + (r" + a + "& 0x80 >> 7) -> r0, set SR",
    PUSH: (__, ___, ____, _____) => "PUSH | PC+1 -> ST++, set SR",
    POP: (__, ___, ____, _____) => "POP | --ST -> PC, set SR",
    HLT: (__, ___, ____, _____) => "HLT | stop program execution",
}

export function getInstructionName(opcode) {
    return names[opcode % specs.numInstructions];
}

export function formatInstruction(value) {
    const opcode = value >> specs.wordSize;
    const args = value & ((1 << specs.wordSize) - 1);
    const argsHi = value >> (specs.wordSize / 2);
    const argsLo = value & ((1 << (specs.wordSize / 2)) - 1);

    const name = getInstructionName(opcode);
    const func = funcs[name];

    return func(opcode, args, argsHi, argsLo);
}
