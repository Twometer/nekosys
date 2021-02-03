bits 16     ; 16-bit 
org 0x8000  ; Stage 2 Loader offset

;;;;;;;;;;;;;;;;;;;
;; Configuration ;;
;;;;;;;;;;;;;;;;;;;
%define debug_log     0

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
init:
    mov [disk_num], dl  ; Save disk number

    call clear_screen   ; Clear VGA terminal
    push log_welcome    ; Show welcome message
    call print

    mov ax, [boot_sector + 0x1BE + 0x08]    ; Load first partition LBA from MBR
    mov [partition_lba], ax

    push word [partition_lba]               ; Load FAT header from disk
    push sector_cache
    call read_sector

    push log_readfat                        ; Show fat parsing log
    call print

    mov ax, [sector_cache + 0x0E]           ; Load number of reserved sectors
    mov [reserved_sectors], ax

    xor ax, ax                              ; Load number of sectors per cluster
    mov al, [sector_cache + 0x0D]
    mov [sectors_per_cluster], ax

    ; Compute start of root directory
    mov ax, [sector_cache + 0x16]           ; AX = blocksPerFat
    mov bx, [sector_cache + 0x10]           ; BX = numFats
    mul bx
    add ax, [sector_cache + 0x0E]           ; AX += numReservedBlocks
    add ax, [partition_lba]                 ; AX += partitionLba
    mov [rootdir_start_lba], ax             ; rootdirStartLBA = AX

    ; Compute end of root directory
    mov ax, [sector_cache + 0x11]           ; numRootDirEntries
    mov bx, 32                              ; 32 bytes per dir entry
    mul bx                                  ; ax *= 32
    mov bx, 512                             ; 512 bytes per sector
    div bx                                  ; ax /= 512
    add ax, [rootdir_start_lba]             ; ax += rootdirStartLba
    mov [rootdir_end_lba], ax               ; rootdirEndLba = ax

    ; Load FAT table
    xor ax, ax                              ; Configure destination for read_sectors
    mov es, ax                              ; Dest segment
    mov bx, fat_table                       ; Dest address

    mov ax, [partition_lba]                 ; Calculate FAT table LBA in AX
    add ax, [reserved_sectors]
    push ax
    push word 16                            ; Read 8K (16 sectors)
    call read_sectors

    ; Scan root directory for kernel file
    push word [rootdir_start_lba]
    push word sector_cache
    call read_sector

    mov bx, sector_cache     ; Directory parser pointer
    next_dirent:

    cmp byte [bx], 0xE5      ; If the first byte is 0xE5, skip it because it's unused
    je dirent_continue

    mov ah, [bx + 11]        ; Load directory flags to AH
    and ah, 0x02             ; And with 0x02 to check if it's hidden
    cmp ah, 0  
    jne dirent_continue      ; If it's hidden, skip it
    
    push bx                  ; Did we find the kernel image?
    push kernel_file
    call streq
    cmp ax, 1                ; streq() retruns its result in AX
    jne dirent_continue

    push bx                  ; We found the kernel :3
    call print
    push newline
    call print

    mov ax, [bx + 26]        ; Store the cluster of the file
    jmp kernel_found

    dirent_continue:
    add bx, 32               ; Dir entries are 32 bytes in size, go to next one.
    cmp byte [bx], 0
    jne next_dirent

    ; If we reach here, the kernel was not found.
    jmp on_not_found

    ; But if we get here, we found it.
    kernel_found:
    push log_success      ; Declare success! :P
    call print

    mov bx, kernel_offset ; Set the destination pointer for read_sectors

    load_next_cluster:
%if debug_log
    push ax               ; Print current cluster
    call printhex
%endif

    call cluster2Sector   ; Converts Cluster(AX) -> Sector(CX)
    
%if debug_log
    push bx               ; Print current cluster's base sector
    call printhex
%endif

    push cx               ; The sector
    push word [sectors_per_cluster] ; Number of sectors
    call read_sectors     ; Loads to ES:BX and increments as needed

    call next_cluster     ; Get next cluster in the chain, operates on AX

    cmp ax, 0xFFFF        ; Check if we reached the end of the chain
    jne load_next_cluster

    jmp kernel_offset     ; Transfer control to the kernel

    jmp halt_system

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void next_cluster()               ;;
;;                                   ;;
;; Takes a cluster in AX and returns ;;
;; the next one in AX                ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
next_cluster:
    push bx

    mov bx, ax
    add bx, bx
    add bx, fat_table
    mov ax, [bx]

    pop bx
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void cluster2Sector()           ;;
;;                                 ;;
;; Takes cluster in AX and returns ;;
;; sector in CX                    ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cluster2Sector:
    push ax
    push bx
    
    sub ax, 2                       ; AX -= 2  (FAT16 starts assigning clusters after #2)
    mov bx, [sectors_per_cluster]   ; AX *= clusterSize
    mul bx

    add ax, [rootdir_end_lba]       ; AX += endOfRootdir
    mov cx, ax                      ; Return in CX

    pop bx
    pop ax
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; bool streq(word *a, word *b) ;;
;;                              ;;
;; Returns 1 or 0 in AX         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
streq:
    push bp
    mov bp, sp
    push si
    push di

    mov si, [bp + 6] ; Load address for left side
    mov di, [bp + 4] ; Load address for right side

    streq_next_char:
    mov al, [si] ; Load left to AL
    mov ah, [di] ; Load right to AH

    cmp ax, 0    ; If both are NULs, we're done and equal
    jz streq_done_eq

    cmp ah, al   ; Check the chars for equality
    jne streq_done_neq

    inc si       ; If they're equal, goto next char
    inc di
    jmp streq_next_char 

    streq_done_neq:
        mov ax, 0
        jmp streq_exit

    streq_done_eq:
        mov ax, 1

    streq_exit:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret 4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void print(word *data) ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
print:
    push bp
    mov bp, sp
    pusha

    mov si, [bp + 4] ; String index
    mov ah, 0x0e     ; TTY mode

    print_nextchar:
    mov al, [si]     ; Load char to write
    int 0x10
    inc si

    cmp byte [si], 0 ; Check end of string
    jnz print_nextchar

    popa
    mov sp, bp
    pop bp
    ret 2 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void printchr(word char) ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
printchr:
    push bp
    mov bp, sp
    pusha

    mov ah, 0x0e            ; TTY mode
    mov byte al, [bp + 4]   ; Character to write
    int 0x10

    popa
    mov sp, bp
    pop bp
    ret 2 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void printhex(word num) ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
printhex:
    push bp
    mov bp, sp
    pusha

    mov ax, [bp + 4] ; Load number

    ; 1st nibble
    mov bx, 0
    mov bl, ah
    and bl, 0xf0
    mov cl, 4
    shr bl, cl
    add bx, hextable
    push word [bx]
    call printchr

    ; 2nd nibble
    mov bx, 0
    mov bl, ah
    and bl, 0x0f
    add bx, hextable
    push word [bx]
    call printchr

    ; 3rd nibble
    mov bx, 0
    mov bl, al
    and bl, 0xf0
    mov cl, 4
    shr bl, cl
    add bx, hextable
    push word [bx]
    call printchr

    ; 4th nibble
    mov bx, 0
    mov bl, al
    and bl, 0x0f
    add bx, hextable
    push word [bx]
    call printchr

    ; EOL
    push newline
    call print

    popa
    mov sp, bp
    pop bp
    ret 2 

;;;;;;;;;;;;;;;;;;;;;;;;
;; Disk access packet ;;
;; for LBA BIOS calls ;;
;;;;;;;;;;;;;;;;;;;;;;;;
disk_packet:
               db 0x10 ; size of packet (16 bytes)
               db 0x00 ; always zero
num_blocks:    dw 1   
dst_addr:      dw 0x00 ; memory dst address
dst_seg:       dw 0x00 ; memory dst segment
src_lba:       dd 0x00 ; lba location
               dd 0x00

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void read_sectors(word lba, word num)             ;;
;;                                                   ;;
;; Will write the sectors to ES:BX and increase ES   ;;
;; and BX as neccessary                              ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_sectors:
    push bp
    mov bp, sp
    push ax
    push cx
    push dx

    mov dx, [bp + 4]    ; Remaining sectors to read
    mov cx, [bp + 6]    ; Source LBA
    
    rds_next_sector:

    push cx
    push bx
    call read_sector

    add bx, 512         ; destAddress += 512
    cmp bx, 0           ; Did we cross a segment border?
    jne rds_continue    ; If not, then just continue

    mov ax, es          ; If we did, increase the segment by 0x1000
    add ax, 0x1000
    mov es, ax

    rds_continue:
    inc cx              ; ++sourceLba
    dec dx              ; --remainingSectors
    cmp dx, 0
    jnz rds_next_sector ; Check if we have remaining sectors

    pop dx
    pop cx
    pop ax
    mov sp, bp
    pop bp
    ret 6

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void read_sector(word lba, word *dest) ;;
;;                                        ;;
;; Will write the sector to ES:dest       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_sector:
    push bp
    mov bp, sp
    pusha

    mov word [num_blocks], 1  ; Configure disk packet:
    mov [dst_seg], es         ; Load dest segment
    
    mov ax, [bp + 4]          ; Dest address
    mov [dst_addr], ax

    mov ax, [bp + 6]          ; Source LBA
    mov [src_lba], ax
    
    mov si, disk_packet       ; Send disk packet
    mov ah, 0x42
    mov dl, [disk_num]
    int 0x13
    
    jc on_disk_error          ; Catch errors

    popa
    mov sp, bp
    pop bp
    ret 4

;;;;;;;;;;;;;;;;;;;;;;;;
;; void clearScreen() ;;
;;;;;;;;;;;;;;;;;;;;;;;;
clear_screen:
    push bp
    mov bp, sp
    pusha

    mov ah, 0x07   ; scroll down
    mov al, 0x00   ; full window
    mov bh, 0x07   ; white on black
    mov cx, 0x00   ; origin is 0|0
    mov dh, 0x18   ; 18h = 24 rows
    mov dl, 0x4f   ; 4fh = 79 cols
    int 0x10       ; video int

    mov ah, 0x02   ; cursor pos
    mov dx, 0x0000 ; set to 0|0
    mov bh, 0x00   ; page 0
    int 0x10       ; video int

    popa
    mov sp, bp
    pop bp
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Error condition targets ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
on_not_found:
    push err_not_found
    call print
    jmp halt_system

on_disk_error:
    push err_disk_io
    call print
    jmp halt_system

halt_system:
    cli
    hlt
    jmp halt_system

;;;;;;;;;;;;;;;;;
;; Disk params ;;
;;;;;;;;;;;;;;;;;
disk_num:            db 0x00
partition_lba:       dw 0x00
rootdir_start_lba:   dw 0x00
rootdir_end_lba:     dw 0x00
reserved_sectors:    dw 0x00
sectors_per_cluster: dw 0x00

;;;;;;;;;;;;;;;;;;;;;;
;; String constants ;;
;;;;;;;;;;;;;;;;;;;;;;
kernel_file: db "koneko.bin", 0
newline: db 0x0a, 0x0d, 0
hextable: db "0123456789ABCDEF"

;;;;;;;;;;;;;;;;;;
;; Log messages ;;
;;;;;;;;;;;;;;;;;;
log_welcome: db "nekosys Bootloader", 0xa, 0xd, 0
log_readfat: db "Reading FAT", 0xa, 0xd, 0
log_success: db "Kernel found", 0xa, 0xd, 0

err_disk_io: db "I/O Error", 0xa, 0xd, 0
err_not_found: db "Kernel not found", 0xa, 0xd, 0