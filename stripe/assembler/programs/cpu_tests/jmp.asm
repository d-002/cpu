.start
    ldi 3
    mov r0,r1
    ldi 5
    mov r0,r2
    ldi .next1
    jmp r0
    ldi 1
    hlt

.next1
    xor r1,r1
    ldi .next2
    jz r0
    ldi .fail
    jmp r0

.next2
    or r1,r2
    ldi .fail
    jz r0

.next3
    sub r1,r2
    ldi .win
    jc r0

.fail
    ldi 1
    hlt

.win
    ldi 0
    hlt
