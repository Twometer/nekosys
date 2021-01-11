section .boot
bits 16
global boot

mmap_state_location: equ 0x8000
mmap_list_location: equ 0x8008

boot:
    ; ** MEMORY MAP LOADER **
    mov dword [mmap_state_location], 0

    ; Set ES:DI target location for memory map
    mov ax, 0            ; segment zero
    mov es, ax
    mov di, 0x8008

    ; Call BIOS
    xor ebx, ebx            ; clear ebx
    mov edx, 0x0534D4150    ; magic number (SMAP)
    next_entry:
    mov eax, 0xe820         ; set command E820
    mov ecx, 24             ; length maybe?
    int 0x15
    jc mmap_err_carry       ; if carry is set, then it failed

    mov edx, 0x0534D4150
    cmp eax, edx
    jne mmap_err_bad_sig    ; if eax is not the magic num then error

    cmp ebx, 0              ; if ebx == 0, it ended
    je mmap_err_list_end

    ; if it didn't fail, goto next entry
    inc dword [mmap_state_location]
    add di, 24 ; increment di by 24
    jmp next_entry  
    
    ; ** MMAP ERROR HANDLERS **
    mmap_err_list_end:
        mov dword [mmap_state_location + 4], 0x01
        jmp continue_boot

    mmap_err_bad_sig:
        mov dword [mmap_state_location + 4], 0x02
        jmp continue_boot

    mmap_err_carry:
        mov dword [mmap_state_location + 4], 0x03
        jmp continue_boot


    ; ** ENTER 32-BIT BOOT **
continue_boot:

    ; Remove 1MB limit
    mov ax, 0x2401
    int 0x15 ; enable A20 bit

    ; Set VGA Text Mode
    mov ax, 0x2
    int 0x10

    ; Load temporary Global Descriptor Table to get into kernel and enter protected mode
    cli
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
