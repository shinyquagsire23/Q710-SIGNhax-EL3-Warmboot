.arm
.align 4
.section ".init"

.extern main
.global _start
.global start

start:
_start:
    ldr sp, =__stack_end__
    bl start_clear_bss
    bl main
    b .

start_clear_bss:
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    sub r1, r1, r0
    mov r2, #3
    add r1, r1, r2
    bics r1, r1, r2
    bxeq lr

    mov r2, #0

start_clear_bss_loop:
    stmia r0!, {r2}
    subs r1, r1, #4
    bne start_clear_bss_loop

    bx lr

.pool
