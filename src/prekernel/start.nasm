bits 32 ;; By default, GRUB loads the kernel in 32-bit mode

extern kernel_main

section .text
global start
start:
    ;; Setup stack
    mov esp, stack_end

    ;; Reset EFLAGS
    push 0
    popf

    push ebx ;; Multiboot information structure.
    push eax ;; Multiboot magic value.
    call kernel_main
    cli
.loop:
    hlt
    jmp .loop

align 16
section .bss
stack_begin:
    resb 1 * 1024 * 1024
stack_end:
