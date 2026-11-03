# stripe

# TODO description

## Specs

- RISC architecture, 15 different instructions
- ROM: 64 10-bits instructions
- Registers: 8 (r0-r7), instructions other than `MOV` write to r0
- 6-bit input port, poll
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
opcode b   a
|0000|000|000|
```

So copying the value from register r1 to r2 would be: `0010001010`

## Instruction set

|opcode|name|description|action|
|-|-|-|-|
|0000|NOP|no operation|do nothing|
|0001|LDI|load immediate|b*8+a -> r0|
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
|1101|IN|read from port|io*4 -> r0|
|1110|HLT|halt program|stop the clock|
|1111|N/A|N/A|N/A, pulsed at the start of instructions|

## Helpful notes

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

## Assembler

An assembler file is provided, look at the example files to see the
capabilities.
I'm too lazy to make an actual doc on the language, it's really not too
different from other assembly languages.

The commands below are ran from the `assembler` subdirectory.

Use [uv](https://docs.astral.sh/uv/) to run the assembler: `uv run assembler.py`
It requires [Litemapy](https://github.com/SmylerMC/litemapy), but this should
not be an issue with uv.

It will check and assemble all files in `programs/*.str` into:

- `assembler/bin/*.bin`: really just text files that represent the memory
  contents of the CPU.
  Not compressed into actual binary files because the instructions are not a
  multiple of 8 bits long.
  This also allows to add optional "comments" at the end of lines, that will be
  displayed in the simulator (see below) for a better view of the program.
- `assembler/schematics/*.litematic`: schematic files, readable with
  [Litematica](https://modrinth.com/mod/litematica). \
  You are advised to symlink the schematics directory for easier access, for
  example:
  `ln -s schematics path/to/.minecraft/schematics/stripe-asm`

If you fear this assembling process will be slow if you have a lot of files,
don't worry: the assembler detects which files should be re-assembled and does
not touch programs whose output files are already present and have a more recent
modification date.
Though, if you feel a program should be re-assembled, feel free to manually
delete the .bin and .litematic files, or preferably to call the assembler with
`-f` / `--force`.

## Simulator

Once a program is assembled in its .bin file, the simulator
(`uv run simulator.py file.bin`, requires
[pygame](https://www.pygame.org/docs/)) can be used to view the state of the CPU
during the execution of a program, and also debug it without having to run the
actual Minecraft CPU.

The controls and information are displayed in the interface.
Additionally, one can click on the screen, registers and input port to alter
their state during the execution of the program.

Note that only the tun mode will stop at HLT instructions, these can be bypassed
when manually stepping through a given program.
