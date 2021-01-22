bits 16    ; 16-bit
org 0x7C00 ; BIOS offset

; entry point
boot:
    ; Reset segment registers
    xor ax, ax
    mov ds, ax ; Data segment
    mov es, ax ; Extra segment
    
    ; Make sure that we are at 0:0x7C00 and the BIOS didn't mess with us
    jmp 0:startup
    startup: equ $

    ; Let's make a stack (4k in size)
    mov ax, 0x0900      
    mov ss, ax
    mov sp, 0x1000

    ; Save the disk number
    mov [disk], dl

    ; Load stage 2
    mov ah, 0x2 ; op
    mov al, 0x2 ; sector count: TWO SECTORS
    mov ch, 0x0 ; cyl
    mov cl, 0x2 ; sector
    mov dh, 0x0 ; head
    mov bx, 0x7E00 ; put it at 0x7E00
    mov dl, [disk] ;diskno
    int 0x13

    jc readfail ; if carry is set: instant error
    cmp ah, 0
    je readok  ; if ah is zero, read

    readfail:
    ; write an error message
    push ioerr
    call print

    cli ; and halt
    hlt

    readok:
    ; write a success message
    push iogood
    call print

    jmp 0x7E00 ; and transfer execution to the big chungus fat16 loader

    ; Shouldn't get here...
    cli ; clear interrupts
    hlt ; halt execution

print:
    push bp     ; save stack frame
    mov bp, sp

    pusha ;save register states to stack

    mov si, [bp+4] ; get argument from stack
    mov ah, 0x0e ; set mode to tty

    nextchar:
    mov al, [si] ; load char
    int 0x10 ; call video int
    inc si ; increment si

    cmp byte [si], 0 ; check if we reached the end
    jne nextchar; if not, write next char

    popa ;load register states back from stack

    mov sp, bp ;return stack frame
    pop bp

    ret

; memory
disk: db 0x0

; messages
ioerr: db "Failed to access boot disk", 0xa,0xd,0
iogood: db "Bootloader loaded",0xa,0xd,0

; boot sector
times 510 - ($-$$) db 0 ; pad to full sector
dw 0xaa55 ; boot signature
