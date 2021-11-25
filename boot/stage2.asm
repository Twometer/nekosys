bits 16     ; We're in 16-bit mode
org 0x8000  ; Stage-2 loader offset

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Memory location definitions ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define boot_sector   0x7C00
%define sector_cache  0x4000
%define fat_table     0x5000
%define kernel_offset 0xA000

;;;;;;;;;;;;;;;;;
;; Entry point ;;
;;;;;;;;;;;;;;;;;
main:
    ; Save the disk
    mov [disk_num], dl

    ; Success message
    push banner
    call print

    ; Parse the MBR
    mov ax, [boot_sector + 0x1BE + 0x08]
    mov [partition_lba], ax

    ; Load the identity table
    push word [partition_lba]
    push sector_cache
    call read_sector

    ; Check for echFS signature
    mov si, echfs_sig
    mov di, sector_cache + 4
    call check_signature
    jc on_fs_error

    ; Allow writes above 1MB
    call enter_unreal

    ; ...

    cli
    hlt


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Takes signature pointers in SI and DI,     ;;
;; and sets the carry bit if they don't match ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
check_signature:
    chksig_nextchar:
        mov ah, [si]
        mov al, [di]

        cmp ah, 0
        je chksig_equal

        cmp al, 0
        je chksig_equal

        cmp ah, al
        jne chksig_nequal

        inc si
        inc di
        jmp chksig_nextchar

    chksig_equal:
        clc
        ret

    chksig_nequal:
        stc
        ret

;;;;;;;;;;;;;;;;;;;;;;;;
;; Enters unreal mode ;;
;;;;;;;;;;;;;;;;;;;;;;;;
enter_unreal:
    cli
    push ds

    ; Enter protected mode
    lgdt [unreal_gdt_info]
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; Stay in real-mode CS and jmp to not crash
    jmp $ + 2

    ; Set data segment to new extended DS
    mov bx, 0x08
    mov ds, bx

    ; Leave protected mode
    mov eax, cr0
    and al, 0xfe
    mov cr0, eax
    pop ds
    sti

    ; Enable A20 line
    mov ax, 0x2401
    int 0x15
    ret

unreal_gdt_info:
    dw unreal_gdt_end - unreal_gdt - 1 ; GDT size
    dd unreal_gdt                      ; GDT address
unreal_gdt: dd 0, 0 ; null descriptor
            db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0 ; big flat data segment
unreal_gdt_end:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Error condition targets ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
on_fs_error:
    push err_file_system
    call print
    jmp halt_system

;;;;;;;;;;;;;;;
;; Libraries ;;
;;;;;;;;;;;;;;;
%include "screen.asm"
%include "disk.asm"

;;;;;;;;;;;;;;;;;
;; Disk params ;;
;;;;;;;;;;;;;;;;;
partition_lba:       dw 0x00

;;;;;;;;;;;;;;;;;;;;;;
;; String constants ;;
;;;;;;;;;;;;;;;;;;;;;;
banner: db "Stage 2 loaded", 0x0a, 0x0d, 0
echfs_sig: db "_ECH_FS_", 0
err_file_system: db "Invalid echFS signature", 0x0a, 0x0d, 0

;;;;;;;;;;;;;
;; Padding ;;
;;;;;;;;;;;;;
times 1024 - ($-$$) db 0 ; Pad to two full sectors