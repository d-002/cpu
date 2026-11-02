# stripe

## specs

- RISC architecture, 13 different instructions
- ROM: 64 10-bits instructions
- Registers: 8 (r0-r7), ALU and immediate instructions write to r0
- Clock speed: 50 ticks (0.2Hz)
- Screen: 8x8 lamp array
- Sequential dual read, store in intermediary buffers
- Fetch, Decode, Read, Execute, optional Writeback

ALU operations set the Z (zero) and C (carry) flags.
There are no operations with carry (ex ADD and not ADC).

The decoded instruction is stored in separate buffers for the 4-bit opcode and
the two 3-bit operands.
The `a` operand is duplicated near the registers for wiring reasons.
Instruction structure (msb):

```
opcode a   b
|0000|000|000|
```

So copying the value from register r1 to r2 would be: `0010001010`

## instruction set

|opcode|name|description|action|
|-|-|-|-|
|0000|NOP|no operation|do nothing|
|0001|LDI|load immediate|a*8+b -> r0|
|0010|MOV|copy reg. value|ra -> rb|
|0011|SUB|8-bit subtraction|ra-rb -> r0|
|0100|XOR|bitwise xor|ra^rb -> r0|
|0101|AND|bitwise and|ra&rb -> r0|
|0110|OR|bitwise or|ra\|rb -> r0|
|0111|ADD|8-bit addition|ra+rb -> r0|
|1000|JMP|unconditional jump|ra -> pc|
|1001|JZ|jump on zero|ra -> pc (if Z set)|
|1010|JC|jump on carry|ra -> pc (if C set)|
|1011|PIX|turn a pixel on|screen[ra,rb] = 1|
|1100|RST|turn the screen off|screen[*] = 0|
|1101|HLT|halt program|stop the clock|
|1110|N/A|N/A|N/A|
|1111|N/A|N/A|N/A, pulsed at the start of instructions|

## helpful notes

As this is a RISC CPU, some useful things have to be implemented via code.
For example, loading an immediate instruction to another register than r0
requires a `mov`:

```asm
  ldi value
  mov r0,destination
```

Conditions can be done with flags manipulation and jumps:

```asm
  xor r1,r2
  jz .equal

.different
  // ...

.equal
  // ...
```

```asm
  sub r1,r2
  jc .less

.more_or_equal
  // ...

.less
  // ...
```

Less or equal / more can be achieved by swapping r1 and r2.
