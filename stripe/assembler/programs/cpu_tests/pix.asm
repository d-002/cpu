ldi 1
mov r0,r1
ldi 2
mov r0,r2

pix r1,r2
pix r2,r1
pix r1,r1
pix r2,r2
rst

ldi 0
pix r0,r0
ldi 7
pix r0,r0
ldi 62
pix r0,r0

hlt
