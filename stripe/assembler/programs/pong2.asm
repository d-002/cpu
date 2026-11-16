# Registers:
# - r0: temp (ldi/alu)
# - r1: left paddle y
# - r2: right paddle y
# - r3: ball x
# - r4: ball y
# - r5: ball dx
# - r6: ball dy
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

.loop
    in
    mov r0,r7

.move_paddles
.left_up
    ldi 4
    and r0,r7
    jz .left_down
    ldi 1
    sub r0,r1
    mov r0,r1
# fallthrough: slow because of an useless check, but saves an instruction

.left_down
    ldi 8
    and r0,r7
    jz .right_up
    ldi 1
    add r0,r1
    mov r0,r1

.right_up
    ldi 16
    and r0,r7
    jz .right_down
    ldi 1
    sub r0,r2
    mov r0,r2

.right_down
    ldi 32
    and r0,r7
    jz .ball
    ldi 1
    add r0,r2
    mov r0,r2

.ball
.bounce_y
# check for bit 8, since when the ball has gone out of bounds it should be 1
    ldi 8
    mov r0,r7
    add r4,r6
    and r0,r7
    jz .bounce_x_wall

# ball is bouncing
    ldi 0
    sub r6,r0
    mov r0,r6

.bounce_x_wall
    add r3,r5
    and r0,r7
    jz .move_ball
    xor r1,r2
    and r0,r3
    xor r0,r1
    mov r0,r7
    ldi 1
    add r0,r4
    sub r7,r0
    mov r0,r7
    ldi 2
    sub r0,r7
    jc .init

.hit
    ldi 0
    sub r5,r0
    mov r0,r5

.move_ball
    add r3,r5
    mov r0,r3
    add r4,r6
    mov r0,r4

.render
    rst
# left paddle
    ldi 0
    mov r0,r7
    pix r7,r1
    add r5,r1
    pix r7,r0
    sub r5,r1
    pix r7,r0
# right paddle
    ldi 7
    mov r0,r7
    pix r7,r2
    add r5,r2
    pix r7,r0
    sub r5,r2
    pix r7,r0
# ball
    pix r3,r4

    jmp .loop
