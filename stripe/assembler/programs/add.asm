# first number
    ldi 19
    mov r0,r1
# second number
    ldi 23
    mov r0,r2
    add r1,r2

# expected result and assert
    mov r0,r1
    ldi 42
    xor r0,r1
    jz .ok

.not_ok
    ldi 1
    hlt
.ok
    ldi 0
.end
    hlt
