# Specifications

## General

PUSSI-8 handles mostly 8-bit data, or handles only 8-bit chunks of data when
applicable (see jumps and timer instructions).

PUSSI-8 can run 32 different instructions, listed in the following section in
order of opcode (0 through 31 included).

This means an instruction can be 16-bits, split into 8 high bits for the opcode
and 8 low bits for the operand(s).

The operand will either be considered as one 8-bit immediate value, or two 4-bit
operands (like register addresses).

In some operations and for wiring purposes, some operations may only use one of
the two given 4-bit operands, like NOT.
In this case, the other operand may be anything, but should be set to 0.

Opcodes, operands, as well as practically everything else is clamped to the
desired bit size.
For example, as there are only 8 registers only 3 bits are required to address
them.
This means accessing register 11 is the same as accessing register 3.

> [!WARNING]  
> The operations exposed here mainly target the format for the custom assembler,
> they might not reflect how they are actually encoded inside PUSSI-8.
>
> For example, the IN instruction starts with the address to an I/O port in the
> assembler, while the top operand bits in the real computer are for the
> destination register.
>
> A Google sheets may become available in the future for easier access to this
> information.

## Registers, main memory

Most data manipulation is done through registers.
PUSSI-8 has 8 general-purpose registers, labeled from 0 to 7, which support
simulated dual-read for operations.

For longer-term storage and when in need of additional storage space, one may
use the RAM.
The RAM is made of 256 bytes, ordered in 8 byte pages.
When accessing an element in the RAM, the associated page will be loaded into
one of two cache modules.
In case of cache miss, the module that was the least recently used is
overwritten.

This means that when one wishes to update data inside the RAM, this data is not
actually written to to the RAM until the associated page cache is stored back
into it.

Pages contain 8 consecutive addresses.
Addressing within a page uses the bottom 3 bits of the full address, while the
page address uses the top 5 bits of it.

As a rule of thumb, writable data locations are never set back to 0 when
resetting PUSSI-8.

## External ports

PUSSI-8 has 10 8-bit input/output ports, that can be accessed through IN and OUT
instructions.

## Call stack and program counter

PUSSI-8 includes a 10-deep call stack that stores 16-bit instruction addresses.

As an additional mention, the integrated PUSSI-8 rom only allows 512 2-byte
instructions, while one may address 16 bits of instructions.
This limitation can be circumvented by the use of an external ROM, which can be
easily plugged into the computer as long as it respects the input and output
formats.
When an external ROM is not plugged in, a lever toggle makes the computer read
NOP instructions when out of the first 512 instructions bounds.

The assembler does not currently support exporting to such an external device.

## Misc

At the start of every instruction, an 8-instruction page cache is loaded from
the hex-encoded instruction memory.
Right after that, the program counter is increased by one, to allow jump and pop
operations to work without side effects.

PUSSI-8 has a boolean condition buffer, updated by the COND instruction and used
to determine whether the following jump instructions execute the jump or not.

The computer also features a state register, which is an 8-bit number whose bits
represent the state of the last operation made to the ALU or call stack:

- Bit 128: Z, true when the last ALU operation resulted in a **Z**ero
- Bit 64: C, true when the last ALU operation resulted in a **C**arry
- Bit 32: N, true when the last ALU operation resulted in a **N**egative number
  (top bit is 1)
- Bit 16: V, true when the last ALU operation resulted in a signed o**V**erflow
  (calculated as `(sign_a == sign_b) && (sign_a ^ sign_res)` where sign refers
  to the top bit of a number)
- Bit 8: O, true when the last call stack operation resulted in an **O**verflow
- Bit 4: U, true when the last call stack operation resulted in an **U**nderflow
- Bit 2: 1, always true
- Bit 1: undefined and unused internally, but in reality false

# Instruction set

PUSSI-8 can run 32 different instructions, listed below in order of opcode (0
through 31 included).

The provided assembler also supports quality-of-life aliases, exposed [here](
https://github.com/d-002/cpu/blob/main/pussi-8/assembler/aliases.md).

## NOP

Do nothing.
Arguments: nothing.

Example:

```psi
    nop
```

## COND

Update the condition buffer.
The value of this buffer is true when at least one of the components from a
bitwise AND between the immediate value and the state register is different from
the bottom bit of the immediate value.

Meaning: `condition_buffer = bool(state_register & immediate) ^ (immediate & 1)`

This allows to make complex conditions like "jump when there was no stack
overflow nor underflow, nor the last ALU operation resulted in a negative
number" (in this case `cond 57`)

Argument: 8-bit immediate.

Example:

```psi
    cond 128 ; set the condition buffer to true when the last ALU operation
             ; resulted in result of zero
```

## JUMPI

Jump using the immediate value as the address.
This means it is only possible to jump from addresses 0 to 255 (included) with
this instruction, but it is faster than storing the jump address in two separate
registers like the following instructions.

Argument: 8-bit immediate.

Example:

```psi
    jump 42
```

## JUMP

Jump to the address pointed to by two registers.
The reason two registers are used is because they can only store 8-bit numbers,
but there are $2^16$ possible instructions.

The translation is achieved by taking the value of the register whose address is
the first operand, shifting it by 8 bits to the left and adding it to the
register pointed to by the second operand.

Arguments: two register addresses, the first one is the high bits and the second
one is the low bits of the target address.

Example:

```psi
    ; preliminary steps for demonstration only, load an arbitrary address
    ldi 42,%r1
    ldi 69,%r2

    jump %r1,%r2 ; jump to (42 << 8) + 69
```

## JUMPR

Jump using the immediate value as a relative address (between 0 and 255
included).
This makes it easier to jump forward to a close enough address without the need
for additional registers.

Argument: 8-bit immediate.

Example:

```psi
    ; say the address here is 42
    jumpr 69 ; after this executes, the program counter is at 42 + 69
```

## LDI

Load immediate value to register `%r0`.
If you are confused as to why I add another operand in the examples, please take
a look at the aliases documentation linked above.

Argument: 8-bit immediate.

Example:

```psi
    ldi 42
```

## MOVEI

Move data between registers, pointed to by address.

Arguments: two 4-bit register addresses.

Example:

```psi
    movei %r1,%r2 ; copy the content of %r1 into %r2
```

## MOVEA

Move data between registers, only the operands are addresses to registers that
themselves contain an address to a register.

Arguments: two 4-bit register addresss to registers containing register
addresses.

Example:

```psi
    ; preliminary steps for demonstration only, store addresses in registers
    ldi 3,%r1
    ldi 4,%r2

    movea %r1,%r2 ; move the content of %r3 into %r4
```

## RTC

Move data from a register into the RAM, taking the address of the source
register as well as the address to a register itself containing the address of
the destination.

Arguments: two 4-bit register addresses, the first being the source and the
second containing the address to an element in the RAM.

Example:

```psi
    ; preliminary step for demonstration only, store address in register
    ldi 3,%r2

    rtc %r1,%r2 ; copy the data in %r1 into %m3
```

## CTR

Move data from the RAM into a register, taking the address to a register itself
containing the address of the source in the RAM, as well as an address to the
destination register.

Arguments: two 4-bit register addresses, the first being containing the address
to an element in the RAM and the second containing the source.

Example:

```psi
    ; preliminary step for demonstration only, store address in register
    ldi 3,%r1

    rtc %r1,%r2 ; copy the data in %m3 into %m2
```

## TIMER

Read an 8-bit range from the 32-bit timer into a register.

The 8-bit range is chosen among the 4 available ones by using the bottom bits of
the operand, while the top ones refer to a register

Arguments: a register address for the top bits and a number from 0 to 3
included.

Example:

```psi
    timer %r0,1 ; read the range of bits from bits 9 to 16 into %r0
```

## MUL

Multiply two register values and store the result into `%r0`.

Arguments: two 4-bit register addresses.

Example:

```psi
    mul %r1,%r2
```

## DIV

Divide two register values and store the result into `%r0`.

In case of a division by zero, the number returned is undefined.

Arguments: two 4-bit register addresses.

Example:

```psi
    div %r1,%r2
```

## MOD

Store the result of the remainder of the division of the first operand by the
second into `%r0`.

In case of a division by zero, the number returned is undefined.

Arguments: two 4-bit register addresses.

Example:

```psi
    nop
```

## IN

Read from an I/O port into a register.
The I/O port's address is the one contained inside the source register.

Arguments: two 4-bit register addresses, the first one containing an address to
the source I/O port, and the second one containing the address to the
destination register.

> [!WARNING]  
> Notice a difference between IN and the following OUT instructions: for IN, the
> destination register address is contained inside the register passed as
> operand, while for OUT its address is directly in the operand.
>
> While this may be confusing, it was done this way to make the computer wiring
> for this part significantly easier.

Example:

```psi
    ; preliminary step for demonstration only, store addresses in registers
    ldi 3,%r1
    ldi 4,%r2

    in %r1,%r2 ; copy the value in port %p3 into register %r4
```

## OUT

Write from a register into an I/O port.
The I/O port's address is the one contained inside the destination register.

Arguments: two 4-bit register addresses, the first one being for the source
register and the second one containing the address to the destination port.

Example:

```psi
    ; preliminary step for demonstration only, store address in register
    ldi 3,%r2

    out %r1,%r2 ; copy the value in register %r1 into port %p3
```

## OR, NOR, ADD, SUB, XOR, XNOR, AND, NAND

Execute the corresponding ALU binary operation with the two given operands and
store the result in `%r0`.
The ALU part of the state register (bits 16 to 128) is updated accordingly.

The operands are executed in reading order, meaning if `%r1` contains 5 and
`%r2` contains 3, then `sub %r1,%r2` will contain 2.

Arguments: two 4-bit register addresses.

Example:

```psi
    add %r1,%r2
```

## NOT, LSH, RSH, ROR, ROL

Execute the corresponding ALU unary operation with the given operand and store
the result in `%r0`.
The ALU part of the state register (bits 16 to 128) is updated accordingly.

Arguments: one 4-bit register address.

Example:

```psi
    not %r1
```

## PUSH

Push `program_counter` (after the increment at the start of the current
instruction) to the top of the stack.
If the stack is full, do not push anything (obviously).
The call stack part of the state register (bits 2 and 4) is updated accordingly.

Arguments: nothing.

Example:

```psi
    push
```

## POP

Pop the top of the stack into the program counter.
If the stack is empty, do not jump.
The call stack part of the state register (bits 2 and 4) is updated accordingly.

Example:

```psi
    pop
```

## HALT

Halt program execution by stopping the clock.
Notice: in the emulator, this does not apply in step mode.

Arguments: nothing.

Example:

```psi
    halt
```

