.start
    ldi 3
    mov r0,r1
    ldi 5
    mov r0,r2
    ldi 1

    jmp .next1
    hlt

.next1
    xor r1,r1
    jz .next2
    hlt

.next2
    or r1,r2
    jz .fail

.next3
    sub r2,r1
    jc .next4
    hlt

.next4
    sub r1,r2
    jc .fail

.win
    ldi 0
.fail
    hlt
