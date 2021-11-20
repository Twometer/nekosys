bits 16     ; We're in 16-bit mode
org 0x7C00  ; The BIOS offset

%define stage2_dst_address 0x8000    ; Destination address
%define stage2_src_sector  1         ; Source sector
%define stage2_src_size    2         ; Size, in sectors

;;;;;;;;;;;;;;;;;
;; Entry point ;;
;;;;;;;;;;;;;;;;;
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

    ; Save the disk
    mov [disk_num], dl

    ; Screen init
    call clear_screen

    push banner
    call print

    ; Load stage 2
    mov bx, stage2_dst_address
    push stage2_src_sector
    push stage2_src_size
    call read_sectors

    ; Enter stage 2
    jmp stage2_dst_address

    ; Halt if we get here
    cli
    hlt

;;;;;;;;;;;;;;;
;; Libraries ;;
;;;;;;;;;;;;;;;
%include "screen.asm"
%include "disk.asm"

;;;;;;;;;;;;;;;;;;;;;;
;; String constants ;;
;;;;;;;;;;;;;;;;;;;;;;
banner: db "nekosys bootloader", 0x0a, 0x0d, 0

;;;;;;;;;;;;;
;; Padding ;;
;;;;;;;;;;;;;
times 510 - ($-$$) db 0 ; Pad to full boot sector
dw 0xaa55               ; Boot signature