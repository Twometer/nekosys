bits 16		; still 16 bit
org 0x7E00  ; loader offset

; Memory layout
; 
; [0x7C00] +200h: Boot sector and stage 1 loader
; [0x7E00] +400h: Stage 2 bootloader (this one here). May expand in the future
; [0x8200] +...: Stores some FAT sectors and finally the kernel code. May move in the future.

fat_offset: equ 0x8200

init:
	; get disk parameters
	mov [disk], dl ; save disk no

	mov ah, 8
	int 0x13
	mov [num_heads], dh
	add dh, 1
	and cl, 0x3f
	mov [sectors_per_track], cl

	; loader main
	call clearscreen
	
	push log_header
	call print
	
	push log_welcome
	call print
	
	push log_readfat
	call print
	
	; Read the FAT system	
	
	; 7C00h is where the boot sector is, 1BEh is where the first partition is
	; and 0x08 is the offset of the LBA where it starts, deref that and we have the 
	; sector of the first partition. load that into fat_offset
	mov ax, [0x7C00 + 0x1BE + 0x08]
	mov [first_partition_offset], ax
	
	push word [first_partition_offset]
	push fat_offset
	call read_sector
	
	; compute location of data region sector
	mov ax, [fat_offset + 0x16] ; blocks per fat: 0x16
	mov bx, [fat_offset + 0x10] ; num of fats: 0x10
	xor dx, dx ; clear dx register
	mul bx ; ax *= bx
	add ax, [fat_offset + 0x0e] ; num_reserved blocks: 0x0e
	add ax, [first_partition_offset]; add partition offset
	
	push ax ; ax now holds the sector with the data region
	push fat_offset ; load that again to fat_offset
	call read_sector
	
	; read the root directory here:
	mov bx, fat_offset ; start reading at the beginning
	
	read_dirent:
	mov al, [bx+11] ; al now contains the flags

	mov ah, al
	and ah, 0x2 ; and with hidden flag mask
	cmp ah, 0x00 ; check if the result is != zero (which means it is set)
	jne next_entry ; if hidden flag is set, dont show entry!

	cmp byte [bx], 0xE5 
	je next_entry ; if first byte of dir entry is 0xe5, that entry is unused
	
	push bx
	push kernel_file
	call streq
	cmp ax, 0 ; if not equal, then AX=0 and we have not found our kernel file, goto next entry
	jz next_entry
	
	push bx		; print the name of the found kernel directory entry
	call print 
	
	push log_found ; and a log statement to complete the line
	call print
	
	; jump to kernel found routine
	mov ax, [bx+26] ; store the cluster number of the file in AX
	sub ax, 2 ; the cluster is 2-based, so fix that
	jmp kernel_found
	
	next_entry:	
	add bx, 32 ; move forward by 32 bytes (size of dirent)
	cmp byte [bx], 0
	jne read_dirent ; if the first byte of dir entry is null, then reading is done
	
	; if we exit this loop, the kernel was not found :c
	push err_nokrnl
	call print
	jmp halt_system
	
	; but if we reach here, the kernel was found :3
	kernel_found:
	push log_ldkernel
	call print
	
	; let's compute the sector of that file
	
	; Load C kernel to RAM
	
	; push log_done
	; call print
	; Jump to kernel
	
	jmp halt_system
	
; disk parameters
disk: db 0x0
num_heads: dw 0x0000
sectors_per_track: dw 0x0000
first_partition_offset: dw 0x0000


; jmp targets
on_disk_error:
	push err_diskio
	call print
	jmp halt_system

halt_system:
	cli
	hlt
	jmp halt_system
	
; subroutines
streq: ; (word aPtr, word bPtr)
	push bp ; stack frame
	mov bp, sp
	
	push bx ; save bx and di register on stack
	push di
	
	mov bx, [bp+6] ; address ctr for left side
	mov di, [bp+4] ; address ctr for right side
	
	compare_char:
	mov al, [bx] ; load left char to al
	mov ah, [di] ; load right char to ah
	
	cmp al, 0 ; if we reached a NUL-byte, we are done and equal
	je done_eq
	cmp ah, 0
	je done_eq
	
	cmp ah, al ; compare the char
	jne done_noteq ; if not equal, we exit
	
	inc bx ; if equal, compare next char
	inc di
	jmp compare_char
	
	done_noteq:
	mov ax, 0
	jmp streq_end
	
	done_eq:
	mov ax, 1
	
	streq_end:
	pop di ; return bx and di back from the stack
	pop bx
	
	mov sp, bp ; restore stack frame
	pop bp
	ret 4
		
read_sector: ; (word sector, word destPtr)
	push bp     ; save stack frame
	mov bp, sp
	

	pusha ;save register states to stack
	
	; compute chs addr
	; i wrote this and it worked first try - wtf
	mov ax, [bp+6] ; divisor (LBA sector)
	mov bx, [sectors_per_track] ; dividend
	xor dx, dx ; clear dx
	div bx ; divide ax / bx
	; ax: quot
	; dx: rem
	mov cl, dl ; CL = sector = (lba % sectors_per_track)
	inc cl ; 											+1
	
	mov bx, num_heads ; divide temp (result of last div, in ax) by num_heads
	xor dx, dx ; clear dx
	div bx ; divide and remainder now in dx which is head num, quot is cylinder
	mov ch, al ; cylinder
	mov dh, dl ; head
	
	; stack grows upwards, so bp+6 is the 1st arg and bp+4 is the 2nd
	
	; after we computed that address and stored it in the correct registers,
	; call the 0x13 interrupt to read from disk
	mov ah, 0x2	   ; op
	mov al, 0x1    ; sector count
	mov bx, [bp+4] ; dst address
	mov dl, [disk] ;diskno
	int 0x13
	jc on_disk_error ; carry flag means error
	
	
	popa ;load register states back from stack
	
	mov sp, bp ;return stack frame
    pop bp
		
	ret 4 ; clean stack and return
	
print: ; (word msgPtr)
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
		
	ret 2 ; clean stack and return
	
clearscreen: ; ()
	push bp     ; save stack frame
	mov bp, sp
	
	pusha
	
	; clear screen
	mov ah, 0x07 ; scroll down
	mov al, 0x00 ; full window
	mov bh, 0x07 ; white on black
	mov cx, 0x00 ; origin is 0|0
	mov dh, 0x18 ; 18h = 24 rows
	mov dl, 0x4f ; 4fh = 79 cols
	int 0x10 ; video int
	
	; reset cursor
	mov ah, 0x02 ;cursor pos
	mov dx, 0x0000 ;set to 0|0
	mov bh, 0x00 ; page 0
	int 0x10 ;video int
	
	popa
	
	mov sp, bp ;return stack frame
    pop bp
	
	ret ; stack is clean, no args, so return

; Boot loader log messages
log_header: db "nekosys Bootloader", 0xa, 0xd, 0
log_welcome: db "(c) 2020 Twometer Applications", 0xa, 0xd, 0
log_readfat: db "Reading file system...", 0xa, 0xd, 0
log_found: db " found", 0xa, 0xd, 0
log_ldkernel: db "Loading kernel...", 0x0a, 0x0d, 0
log_done: db "Entering kernel... bye :3", 0x0a, 0x0d, 0

; Boot loader error messages
err_diskio: db "Disk IO Error!", 0xa, 0xd, 0
err_nokrnl: db "Kernel file not found!", 0xa, 0xd, 0

; Other strings
kernel_file: db "NEKOLD", 0