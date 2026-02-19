# List of aliases

## Memory management

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

## Jumps

JUMPI and JUMPR stay unchanged, but JUMP can be substituted by something else in
case the argument is not a pair of registers but rather one register or an
immediate value:
- JUMPI if the address is less than 256
- JUMPR if the relative address is between 0 and 256
- JUMP with %r0 and %r1 filled with the top and bottom bits respectively

Additionally, there are some qol jump instructions to hide cond instructions:

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
    cond 17
    jump target
    ```

- `jvc target` (jump on signed overflow unset)
    ```
    cond 16
    jump target
    ```

- `jos target` (jump on stack overflow set)
    ```
    cond 9
    jump target
    ```

- `joc target` (jump on stack overflow clear)
    ```
    cond 8
    jump target
    ```

- `jus target` (jump on stack underflow set)
    ```
    cond 5
    jump target
    ```

- `juc target` (jump on stack underflow set)
    ```
    cond 4
    jump target
    ```

- `j target` (unconditional jump)
    ```
    cond 2
    jump target
    ```
