section .boot
bits 16
global boot

; Kernel Handover header struct definition (short krnlh)
krnlh_mmap_state: equ 0x0500
krnlh_mmap_length: equ 0x0504
krnlh_mmap_ptr: equ 0x0508

krnlh_vesa_length: equ 0x050C
krnlh_vesa_state: equ 0x0510
krnlh_vesa_info_ptr: equ 0x0514
krnlh_vesa_mode_ptr: equ 0x0518
krnlh_vesa_current_mode: equ 0x51C

; Array contents
mmap_content_base: equ 0x600
vesa_info_block: equ 0x1000
vesa_mode_array: equ 0x2000

; Video mode config
;  This is currently hardcoded, as
;  I don't want to implement VM86
;  to change it dynamically. Maybe
;  at some point in the future...
%define requested_x_resolution 1024
%define requested_y_resolution 768
%define force_text_mode_vga 0

; Other definitions
%define mode_info_struct_size 50

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ENTRY POINT FROM BOOTLOADER ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
boot:
; INIT HANDOVER STRUCT
    mov dword [krnlh_mmap_state], 0
    mov dword [krnlh_mmap_length], 0
    
    mov dword [krnlh_vesa_state], 0
    mov dword [krnlh_vesa_length], 0
    
    mov dword [krnlh_vesa_info_ptr], vesa_info_block
    mov dword [krnlh_vesa_mode_ptr], vesa_mode_array
    mov dword [krnlh_mmap_ptr], mmap_content_base

; MMAP LOADER
    ; Set ES:DI target location for memory map
    mov ax, 0            ; segment zero
    mov es, ax
    mov di, mmap_content_base

    ; Call BIOS
    xor ebx, ebx            ; clear ebx
    mov edx, 0x0534D4150    ; magic number (SMAP)
    mmap_next_entry:
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
        inc dword [krnlh_mmap_length]
        add di, 24 ; increment di by 24
        jmp mmap_next_entry  
    
    ; ** MMAP ERROR HANDLERS **
    mmap_err_list_end:
        mov dword [krnlh_mmap_state], 0x01
        jmp mmap_done

    mmap_err_bad_sig:
        mov dword [krnlh_mmap_state], 0x02
        jmp mmap_done

    mmap_err_carry:
        mov dword [krnlh_mmap_state], 0x03
        jmp mmap_done

    mmap_done:

; VESA LOADER
    xor ax, ax ; Zero ES
    mov es, ax

    mov di, vesa_info_block ; Read info block
    mov ax, 0x4F00
    int 0x10
    cmp ax, 0x004F
    jne vesa_err_readinfo

    ; Segment: vesa_info_block[0x10]
    ; Offset:  vesa_info_block[0x0E]

    mov bx, [vesa_info_block + 0x0E]
    mov di, vesa_mode_array
    vesa_next_entry:
        mov ds, [vesa_info_block + 0x10] ; Load video mode entry
        mov cx, [bx]
        xor ax, ax ; Reset data segment
        mov ds, ax

        cmp cx, 0xFFFF ; Reached the end?
        je vesa_list_finished

        ; Save the current mode id in the mode struct
        mov [di], cx
        add di, 2

        ; Load more info about that mode from VESA bios
        mov ax, 0x4F01
        int 0x10
        cmp ax, 0x004F
        jne vesa_err_readmode

        ; Go to next entry
        inc dword [krnlh_vesa_length]
        add di, mode_info_struct_size
        add bx, 2
        jmp vesa_next_entry

; VIDEO MODE SELECTOR
    vesa_list_finished:
        ; Scan the list for a resolution and bpp we want
        mov di, 0
        mov si, vesa_mode_array

        mov cx, 0 ; CX stores the currently highest bit depth
        mov bx, 0 ; BX stores the currently selected vid mode

    vesa_next_res:
        mov ax, [si + 0x02] ; Has linear framebuffer?
        test ax, 128
        jz vesa_loop_continue ; If no linear FB, we don't want the mode

        mov ax, [si + 0x14] ; Check current X resolution
        cmp ax, requested_x_resolution
        jne vesa_loop_continue

        mov ax, [si + 0x16] ; Check current Y resolution
        cmp ax, requested_y_resolution
        jne vesa_loop_continue

        cmp cx, [si + 0x1b] ; Check current bpp
        jg vesa_loop_continue ; If it's more than current max - continue
        mov bx, [si] ; Else, save
        mov cx, [si + 25]

        vesa_loop_continue:
        inc di ; If we reach the end, we stop
        cmp di, [krnlh_vesa_length]
        jge vesa_loop_ended

        ; If we didn't reach the end, goto next mode info block
        add si, 52
        jmp vesa_next_res

    vesa_loop_ended:
        cmp bx, 0 ; If BX is still zero, didn't find our resolution :(
        jne vesa_found_resolution
        mov bx, 0x112 ; Set a default mode (640x480x24)

    vesa_found_resolution:
        ; Call BIOS to change into selected video mode
        mov ax, 0x4F02
        mov [krnlh_vesa_current_mode], bx
        int 0x10
        cmp ax, 0x004F ; Check for errors
        jne vesa_err_chmode
        jmp vesa_done

    vesa_err_readinfo:
        mov dword [krnlh_vesa_state], 0x01
        jmp vesa_done

    vesa_err_readmode:
        mov dword [krnlh_vesa_state], 0x02
        jmp vesa_done

    vesa_err_chmode:
        mov dword [krnlh_vesa_state], 0x03
        jmp vesa_done


    vesa_done:

%if force_text_mode_vga
    mov dword [krnlh_vesa_current_mode], 0x00
    mov ax, 0x2
    int 0x10
%endif

    ; Remove 1MB limit
    mov ax, 0x2401
    int 0x15 ; enable A20 bit

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
