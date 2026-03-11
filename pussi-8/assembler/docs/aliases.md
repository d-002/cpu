# List of aliases

Apart from the [PUSSI-8 instruction set](
https://github.com/d-002/cpu/blob/main/pussi-8/assembler/instruction_set.md),
the [assembler](https://github.com/d-002/cpu/tree/main/pussi-8/assembler)
supports the following quality-of-life instructions:

## Memory management

- `mov source,destination`
    ```
    movei source,destination
    ```

- `ldi data,destination`
    ```
    ldi data
    movei %r0, destination
    ```

- `rtc r,v`
    ```
    movei v,%r0
    rtc r,%r0
    ```

- `ctr r,v`
    ```
    movei v,%r0
    ctr %r0,r
    ```

## Operations

- `alu/mul/div/mod a,b,y`
    ```
    [opcode] a,b
    movei %r0,y
    ```

- `test r`
    ```
    or r,r
    ```

## I/O

- `in port,register`
    ```
    ldi [register value]
    movei %r0,%r7
    ldi [port value]
    in %r0,%r7
    ```

- `out register,port`
    ```
    ldi [port value]
    out register,%r0
    ```

## Jumps

JUMPI and JUMPR stay unchanged, but JUMP can be substituted by something else in
case the argument is not a pair of registers but rather an immediate value
(preferably through a label) or a single register:
- If the argument is a register: %r0 is set to 0, then `JUMP [reg] %r0`
- If the argument is a literal value:
    - JUMPI if the target address is less than 256
    - JUMPR if the relative jump address is between 0 and 256 (excluded)
    - JUMP with %r0 and %r1 filled with the top and bottom bits respectively

Additionally, there are some qol jump instructions to hide cond instructions.
The jumps in them are also subject to be substituted as described above.

- `jz target` (jump if zero)
    ```
    cond 128
    jump target
    ```

- `jeq target` (jump if equal)
    ```
    cond 128
    jump target
    ```

- `jnz target` (jump if nonzero)
    ```
    cond 129
    jump target
    ```

- `jne target` (jump if not equal)
    ```
    cond 129
    jump target
    ```

- `jcs target` (jump on unsigned carry set)
    ```
    cond 64
    jump target
    ```

- `jcc target` (jump on unsigned carry clear)
    ```
    cond 65
    jump target
    ```

- `jpl target` (jump if "plus" (positive))
    ```
    cond 33
    jump target
    ```

- `jmi target` (jump if "minus" (negative))
    ```
    cond 32
    jump target
    ```

- `jvs target` (jump on signed overflow set)
    ```
    cond 16
    jump target
    ```

- `jvc target` (jump on signed overflow unset)
    ```
    cond 17
    jump target
    ```

- `jos target` (jump on stack overflow set)
    ```
    cond 8
    jump target
    ```

- `joc target` (jump on stack overflow clear)
    ```
    cond 9
    jump target
    ```

- `jus target` (jump on stack underflow set)
    ```
    cond 4
    jump target
    ```

- `juc target` (jump on stack underflow set)
    ```
    cond 5
    jump target
    ```

- `j target` (unconditional jump)
    ```
    cond 2
    jump target
    ```
