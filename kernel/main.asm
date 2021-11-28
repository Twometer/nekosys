section .boot
bits 32

; Entry point from bootloader
global _start
_start:
    mov esp, kernel_stack_top
    extern kmain
    call kmain

    cli
    hlt

; Make the Kernel Stack
section .bss
align 4
kernel_stack_bottom: equ $
    resb 16384 ; 16KB stack
kernel_stack_top: