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
- JUMP with %r0 and %r1 filled with the top and bottom bits repsectively

Additionally, there are some qol jump instructions to hide cond instructions:

- `jeq target`
    ```
    cond 128
    jump target
    ```

- `jne target`
    ```
    cond 129
    jump target
    ```

- `jca target`
    ```
    cond 64
    jump target
    ```

- `jnc target`
    ```
    cond 65
    jump target
    ```

- `jpl target`
    ```
    cond 33
    jump target
    ```

- `jmi target`
    ```
    cond 32
    jump target
    ```

- `jvs target`
    ```
    cond 17
    jump target
    ```

- `jvc target`
    ```
    cond 16
    jump target
    ```

- `jos target`
    ```
    cond 9
    jump target
    ```

- `joc target`
    ```
    cond 8
    jump target
    ```

- `jus target`
    ```
    cond 5
    jump target
    ```

- `juc target`
    ```
    cond 4
    jump target
    ```

- `jal target`
    ```
    cond 2
    jump target
    ```
