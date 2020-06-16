section .boot
bits 16
global boot

boot:
    ; Remove 1MB limit
    mov ax, 0x2401
    int 0x15 ; enable A20 bit
    
    ; Set VGA Text Mode
    mov ax, 0x2
    int 0x10 

    ; Load the Global Descriptor Table and enter protected mode
    lgdt [gdt_pointer] ; Load GDT
    mov eax, cr0 
    or eax, 0x1 ; Enable the Protected Mode Bit
    mov cr0, eax
    jmp CODE_SEG:boot32 ; long jump to the code segment
    
; Define the Global Descriptor Table
gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:
    gdt_pointer:
    dw gdt_end - gdt_start
    dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
    
bits 32
boot32:
    ; Make all segments point at the DATA segment we just defined
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Setup stack and enter kernel
    mov esp, kernel_stack_top
    extern nkmain
    call nkmain

    ; When we return from kernel, halt
halt_system:
    cli
    hlt
    jmp halt_system
    
; Our C stack
section .bss
align 4
kernel_stack_bottom: equ $
    resb 16384 ; 16 KB
kernel_stack_top: