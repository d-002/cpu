# cpu

Utilities, world downloads, schematics for my Minecraft Computers and CPUs

## First generation

### [DIKC-8 (summer of 2023)](https://github.com/d-002/cpu/tree/main/dikc-8)

Working computer, with built-in screen and a limited range of operations.
Designed while having basically no knowledge of how computers actually work...

- Program
    - 256B ROM, or 128 2B instructions
    - Reads 1 instruction into a buffer at 10Hz speed
    - 16 possible instructions (1 unused)
    - Conditional jump
- Memory
    - 16 registers
    - 8 "boolean" registers (storing bit by bit)
- I/O
    - Built-in 16x16 screen
- Computing
    - ALU with addition, subtraction, multiplication, division and comparisons
    - Clock stalls during costly operations like multiplication and division
    - Separate LU for boolean logic with the boolean registers
- Misc
    - Word size of 8 bits
    - Limited pipelining
    - 91-tick clock (0.11Hz)
    - Serial observer rom (so not MCHPR compatible)

### [smol DIKC-4 (April 2024)](https://github.com/d-002/cpu/tree/main/smol_dikc-4)

A small project for a small CPU, which fits in a 16x16x16 area!
Unfortunately not Turing-complete, as there was no space to implement
conditional branching.

- Program
    - 8B analog ROM, or 8 1B instructions
    - 7 possible instructions (1 unused)
- Memory
    - 3 registers
- I/O: None
- Computing
    - ALU with addition, subtraction. Saves in dedicated buffer for future use.
- Misc
    - Word size of 4 bits
    - The limited number of instructions can be circumvented by not adding a HLT
      instruction and letting the program loop
    - 50-tick clock (0.2Hz)

### [DIKC-8 2 (2023 - 2025)](https://github.com/d-002/cpu/tree/main/dikc-8_2)

A revamp of the DIKC-8 in every aspect.
It had a major focus on speed and pipelining, which actually ended up killing
the project.
As I wanted to add new features, more components, this speed constraint was an
added complexity that made it harder to debug.

Additionally, as I learned more about computer architecture, I became aware of
the many design flaws in the DIKC-8 2. This ultimately made me abandon the
project and start over, hence the second generation.

- Program
    - 1KB ROM, or 512 2B instructions
    - Reads 1 instruction into a buffer at 10Hz speed
    - 32 possible instructions
    - Conditional and unconditional jump
- Memory
    - 32 registers
    - Buffer for immediate values before writing them to a register
    - 10-deep call stack
- I/O
    - 8 1B ports that can be written to, read from, or pulsed (quickly turn on
      and off again in one single instruction)
    - Outputted 1B ALU result
- Computing
    - ALU operations are stored in dedicated 1B buffer and comparisons in a
      1-bit buffer
    - ALU with addition, subtraction, comparisons, most bitwise operations and
      bit shifts
    - Bitwise operations possible outside the ALU: set or unset a bit, use a bit
      as the condition for the next conditional jump
- Misc
    - Word size of 8 bits
    - Serial ROM for compactness, but split into multiple sections for speed
    - For operations like jumps or division, which take multiple cycles, it is
      possible to run some operations while waiting for the first to finish.
      For example, `LDI ...; JMP ...; NOP` can be made faster by running
      `JMP ...; LDI ...` instead.
    - 33-tick clock (0.3Hz)

> [!WARNING]
> This computer is nonfunctional and discontinued.
> Although I spent a lot of time refining it, the assembly took too much effort
> and its various issues / oversights made it easier to start over, hence the
> second generation.

## Second generation

### [PUSSI-8 (2025 - ...)](https://github.com/d-002/cpu/tree/main/pussi-8)

Born from the ashes of the DIKC-8 2. Made to be better in every way,
incorporating some of my fresh knowledge on computer architecture.

The main motivation for its specs was to make it run Tetris, or even a custom
operating system.

> [!NOTE]
> This computer is currently under creation, most of the following specs are
> subject to change.

- Program
    - 1KB built-in analog ROM, or 512 2B instructions
    - Support for up to 128KB external rom, or 64K instructions
    - 16B decoded instructions cache
- Memory
    - 8 registers with simulated dual-read. Can be used as pointers.
      operations.
    - 16B cache, split into 2 modules to implement LRU. Sole interface between
    the registers and the RAM. Can read and write one address at a time, or in
    bulk.
    - 256B RAM, stored as serial for compactness
    - 10-deep call stack
- I/O
    - 8 1B readable and writable ports
    - 2 reserved output ports (for the ALU buffer as well as the lower bits of
      the program counter)
    - Plug-in external ROM (see ROM section)
- Computing
    - ALU with addition, subtraction, all bitwise operations, bit shifts and
      barrels
    - Big math units for multiplication and division
    - State register for use in conditional jumps: zero, nonzero, carry,
      ALU overflow, stack overflow and underflow, constant 1, constant 0
    - Conditional absolute and relative jumps
- Misc
    - Word size of 8 bits
    - 32 possible instructions (1 unused)
    - Most components notice the caller when they are done, to reduce errors

> [!WARNING]
> Just to be clear, DIKC stands for "D_00's Incredible Keyboardless Computer"
> Same with PUSSI which means "Powerful Unit for Smart Systems of Information"
