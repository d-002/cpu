.init
    ldi 0
    mov r0,r1
    ldi 1
    mov r0,r2

.loop
    add r1,r2
    mov r1,r2
    mov r0,r1
    jmp .loop
