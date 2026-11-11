import enum


class OpcodeEnum(str, enum.Enum):
    NOP = 'NOP'
    LDI = 'LDI'
    MOV = 'MOV'
    SUB = 'SUB'
    XOR = 'XOR'
    AND = 'AND'
    OR = 'OR'
    ADD = 'ADD'
    JMP = 'JMP'
    JZ = 'JZ'
    JC = 'JC'
    PIX = 'PIX'
    RST = 'RST'
    IN = 'IN'
    HLT = 'HLT'
