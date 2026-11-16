# Registers:
# - r0: temp (ldi/alu)
# - r1: left paddle y
# - r2: right paddle y
# - r3: ball x
# - r4: ball y
# - r5: ball movemt, dy: bit 2 is 1 iff moving right, bit 1 if 1 iff moving down
# - r6: temp (all kinds of things)
# - r7: temp (io, ...)
#
# Input:
# - 1 (gives 4): left paddle up
# - 2 (gives 8): left paddle down
# - 4 (gives 16): right paddle up
# - 8 (gives 32): right paddle down

.init
    ldi 3
    mov r0,r1
    mov r0,r2
    mov r0,r3
    mov r0,r4
    ldi 1
    mov r0,r5
    mov r0,r6
    jmp .render # TODO remove

.loop
    in
    mov r0,r7
# cache a 1 for inc / dec later
    ldi 1
    mov r0,r6

.movement
.left_up
    ldi 4
    and r0,r7
    jz .left_down
    sub r6,r1
# fallthrough: slow because of an useless check, but saves an instruction

.left_down
    ldi 8
    and r0,r7
    jz .right_up
    add r6,r1

.right_up
    ldi 16
    and r0,r7
    jz .right_down
    sub r6,r2

.right_down
    ldi 32
    and r0,r7
    jz .ball
    add r6,r2

.ball
.bounce_y
    ldi 1
    and r0,r5
    jz .check_bounce_up
.check_bounce_down
    ldi 7
    xor r0,r4

.check_bounce_up

.render
    rst
# left paddle
    ldi 0
    mov r0,r7
    ldi 1
    mov r0,r6
    pix r7,r1
    add r0,r1
    pix r7,r0
    add r0,r6
    pix r7,r0
# right paddle
    ldi 7
    mov r0,r7
    ldi 1
    pix r7,r2
    add r0,r1
    pix r7,r0
    add r0,r6
    pix r7,r0
# ball
    pix r3,r4

    jmp .loop
