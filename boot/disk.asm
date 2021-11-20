disk_num: db 0x00

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
    ret 4

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Error condition targets ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
on_disk_error:
    push err_disk_io
    call print
    jmp halt_system

halt_system:
    cli
    hlt
    jmp halt_system

;;;;;;;;;;;;;;;;;;
;; Log messages ;;
;;;;;;;;;;;;;;;;;;
err_disk_io: db "I/O Error", 0xa, 0xd, 0