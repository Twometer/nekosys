bits 16     ; We're in 16-bit mode
org 0x8000  ; Stage-2 loader offset

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Memory location definitions ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define boot_sector   0x7C00
%define id_table      0x4000
%define alloc_table   0x4200
%define block_cache   0x5200
%define kernel_tmp    0xA000
%define kernel_dst    0x100000

%define bootinfo_base           0x0500
%define bootinfo_mem_map_result 0x00
%define bootinfo_mem_map_len    0x04
%define bootinfo_mem_map_ptr    0x08 
%define memory_map_base         0x1500

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
    push ax
    push id_table
    call read_sector

    ; Check for echFS signature
    mov si, echfs_sig
    mov di, id_table + 4
    call check_signature
    jc on_fs_error

    ; Load the allocation table (at block 16)
    push word 16
    push word alloc_table
    call read_block

    ; Allow writes above 1MB
    call enter_unreal

    ; Compute offset of main directory
    mov dword eax, [id_table + 12] ; total block count
    mov ebx, 8
    mul ebx
    mov ebx, [id_table + 28] ; block size
    add eax, ebx
    sub eax, 1
    xor edx, edx
    div ebx ; now we have size of alloc table, in blocks
    add eax, 16 ; add offset of alloc table, EAX now stores block# of maindir

    ; read first block of main directory
    push ax
    push block_cache
    call read_block

    mov eax, block_cache
    next_dirent:
        cmp dword [eax], 0
        je kernel_not_found

        mov ebx, eax
        add ebx, 9
        mov si, bx
        mov di, kernel_file_name
        
        call streq
        jnc kernel_found

        add eax, 256
        jmp next_dirent

    kernel_found:
        add eax, 240
        mov eax, [eax]

        push word ax
        push word kernel_tmp
        call read_block

        ; copy kernel block to 1MB
        mov ecx, [id_table + 28] ; block size
        mov esi, kernel_dst
        mov edi, kernel_tmp
    copy_kernel:
        cmp ecx, 0
        je kernel_copy_complete

        mov eax, [edi]
        mov dword [ds:esi], eax
        add edi, 4
        add esi, 4
        sub ecx, 4
        jmp copy_kernel

    ; TODO: Copy more than one block

    kernel_copy_complete:
        ; setup boot info
        call load_mem_map

        ; enter pmode and kernel
        cli
        mov eax, cr0
        or eax, 0x1
        mov cr0, eax
        jmp 0x10:launch_kernel ; leave the bootloader

    kernel_not_found:
        push err_no_kernel
        call print
        cli 
        hlt

bits 32
launch_kernel:
    jmp 0x10:kernel_dst
bits 16


load_mem_map:
    mov dword [bootinfo_base + bootinfo_mem_map_result], 0xFF
    mov dword [bootinfo_base + bootinfo_mem_map_len], 0
    mov dword [bootinfo_base + bootinfo_mem_map_ptr], memory_map_base

    ; Set ES:DI target location for memory map loader
    mov ax, 0
    mov es, ax
    mov di, memory_map_base

    xor ebx, ebx            
    mov edx, 0x0534D4150
    mmap_next_entry:
        mov eax, 0xe820         ; set command E820
        mov ecx, 24             ; entry length
        int 0x15
        jc mmap_err_carry       ; if carry is set, then it failed

        mov edx, 0x0534D4150
        cmp eax, edx
        jne mmap_err_bad_sig    ; if eax is not the magic num then error

        cmp ebx, 0              ; if ebx == 0, it ended
        je mmap_err_list_end

        ; if it didn't fail, goto next entry
        inc dword [bootinfo_base + bootinfo_mem_map_len]
        add di, 24 ; increment di by 24
        jmp mmap_next_entry  
    
    ; Error handlers
    mmap_err_list_end:
        mov dword [bootinfo_base + bootinfo_mem_map_result], 0x00
        ret

    mmap_err_bad_sig:
        mov dword [bootinfo_base + bootinfo_mem_map_result], 0x01
        ret

    mmap_err_carry:
        mov dword [bootinfo_base + bootinfo_mem_map_result], 0x02
        ret

    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void read_block(word block_no, word dst_addr) ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_block:
    push bp
    mov bp, sp
    push eax
   
    xor eax, eax
    mov ax, [bp + 6] ; block no
    mov ebx, [id_table + 28] ; block size
    mul ebx
    xor edx, edx
    mov ebx, 512
    div ebx ; now eax holds offset from partition start in 512byte-sectors
    add ax, [partition_lba] ; add partition offset
    push ax

    mov eax, ebx
    mov ebx, 512
    div ebx; now eax holds size of block in sectors
    push ax

    xor ax, ax
    mov es, ax ; set sector read destination
    mov bx, [bp + 4]

    call read_sectors
    
    pop eax
    mov sp, bp
    pop bp
    ret 4


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

streq:
    push eax
    streq_nextchar:
        mov ah, [si]
        mov al, [di]

        cmp ax, 0
        je streq_equal

        cmp ah, al
        jne streq_nequal

        inc si
        inc di
        jmp streq_nextchar
        
    streq_equal:
        clc
        jmp streq_done

    streq_nequal:
        stc

    streq_done:
        pop eax
        ret

;;;;;;;;;;;;;;;;;;;;;;;;
;; Enters unreal mode ;;
;;;;;;;;;;;;;;;;;;;;;;;;
enter_unreal:
    cli
    push ds
    push es

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
    mov es, bx

    ; Leave protected mode
    mov eax, cr0
    and al, 0xfe
    mov cr0, eax
    pop es
    pop ds
    sti

    ; Enable A20 line
    mov ax, 0x2401
    int 0x15
    ret

unreal_gdt_info:
    dw unreal_gdt_end - unreal_gdt - 1 ; GDT size
    dd unreal_gdt                      ; GDT address
unreal_gdt: dq 0 ; null descriptor (0x00)
            db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0 ; big flat data segment (0x08)
            db 0xff, 0xff, 0, 0, 0, 10011010b, 11001111b, 0 ; big flat code segment (0x10)
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
err_file_system: db "error: invalid echFS signature", 0x0a, 0x0d, 0
err_no_kernel: db "error: kernel.bin not found", 0x0a, 0x0d, 0
kernel_file_name: db "kernel.bin", 0

;;;;;;;;;;;;;
;; Padding ;;
;;;;;;;;;;;;;
times 1024 - ($-$$) db 0 ; Pad to two full sectors