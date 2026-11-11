.start
    ldi 5
    mov r0,r1
    ldi 63
    add r0,r0
    add r0,r0
    mov r0,r2

    jmp .next1
    ldi 1
    hlt

.next1
    xor r1,r1
    jz .next2
    jmp .fail

.next2
    or r1,r1
    jz .fail

.next3
# should carry out because uses the AND logic here (see readme)
    and r2,r1
    jc .next4
    jmp .fail

.next4
    add r1,r1
    jc .fail

.win
    ldi 0
    hlt

.fail
    ldi 1
    hlt
