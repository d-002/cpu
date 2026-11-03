.init
    ldi 0
    mov r0,r1
    ldi 1
    mov r0,r2
    ldi .loop
    mov r0,r4
    ldi .end
    mov r0,r5

.loop
    add r1,r2
    jc r5
    mov r1,r2
    mov r0,r1
    jmp r4

.end
    hlt
