bits 16     ; We're in 16-bit mode
org 0x7C00  ; The BIOS offset

boot:
    ; Reset segment registers
    xor ax, ax
    mov ds, ax 
    mov es, ax
    
    ; Make sure that we are at 0:0x7C00 and the BIOS didn't mess with us
    jmp 0:init
    init: equ $

    ; Initialize a 4K stack
    mov ax, 0x0900      
    mov ss, ax
    mov sp, 0x1000

    mov al, 0x4E ; Show an N on the screen
    mov ah, 0x0E ; TTY mode
    int 0x10

    ; Halt if we get here
    cli
    hlt

times 510 - ($-$$) db 0 ; Pad to full boot sector
dw 0xaa55               ; Boot signature