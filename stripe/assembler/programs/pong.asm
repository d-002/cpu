# 2-player Pong: 110 instructions
# Needs LDI and MOV to leave the Z flag unchanged.
#
# Register use :
# r0 : general stuff
# r1 : ball x (0-7)
# r2 : ball y (0-7)
# r3 : ball x step (1 = right, 255 = left)
# r4 : left bar center y (1-6)
# r5 : right bar center y (1-6)
# r6 : ball y step (1 = down, 255 = up)
# r7 : general stuff
#
# Input : IN gives io*4
# 4 = left up, 8 = left down, 16 = right up, 32 = right down

.init
    ldi 3
    mov r0,r1
    mov r0,r2
    mov r0,r4
    mov r0,r5
    ldi 1
    mov r0,r3
    mov r0,r6

.frame
    in
    mov r0,r7

.left_up
    ldi 4
    and r0,r7
    ldi .left_down
    jz r0
    ldi 1
    xor r4,r0
    ldi .left_down
    jz r0
    ldi 1
    sub r4,r0
    mov r0,r4
.left_down
    ldi 8
    and r0,r7
    ldi .right_up
    jz r0
    ldi 6
    xor r4,r0
    ldi .right_up
    jz r0
    ldi 1
    add r4,r0
    mov r0,r4
.right_up
    ldi 16
    and r0,r7
    ldi .right_down
    jz r0
    ldi 1
    xor r5,r0
    ldi .right_down
    jz r0
    ldi 1
    sub r5,r0
    mov r0,r5
.right_down
    ldi 32
    and r0,r7
    ldi .bounce_y
    jz r0
    ldi 6
    xor r5,r0
    ldi .bounce_y
    jz r0
    ldi 1
    add r5,r0
    mov r0,r5

.bounce_y
    ldi 8
    mov r0,r7
    add r2,r6
    and r0,r7
    ldi .bounce_x
    jz r0
    ldi 0
    sub r0,r6
    mov r0,r6


.bounce_x
    add r1,r3
    and r0,r7
    ldi .move_ball
    jz r0
    xor r4,r5
    and r0,r1
    xor r0,r4
    mov r0,r7
    ldi 1
    add r0,r2
    sub r0,r7
    mov r0,r7
    ldi 3
    xor r0,r7
    ldi .miss
    jz r0
    ldi 12
    and r0,r7
    ldi .hit
    jz r0
    
.miss
    ldi 0
    jmp r0
.hit
    ldi 0
    sub r0,r3
    mov r0,r3
.move_ball
    add r1,r3
    mov r0,r1
    add r2,r6
    mov r0,r2

.draw
    rst
    ldi 0
    mov r0,r7
    pix r7,r4
    add r4,r6
    pix r7,r0
    sub r4,r6
    pix r7,r0
    ldi 7
    mov r0,r7
    pix r7,r5
    add r5,r6
    pix r7,r0
    sub r5,r6
    pix r7,r0
    pix r1,r2
    ldi .frame
    jmp r0
