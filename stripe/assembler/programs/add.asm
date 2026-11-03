# first number
    ldi 10
    mov r0,r1
# second number
    ldi 63
    mov r0,r2
    add r1,r2

# expected result and assert
    mov r0,r1
    ldi 73
    xor r0,r1
    jz .ok

.not_ok
    ldi 1
    hlt
.ok
    ldi 0
.end
    hlt
