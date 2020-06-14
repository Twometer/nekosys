bits 16		; still 16 bit
org 0x7F00  ; loader offset

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
	push 704		; disk sector
	push 0x8000		; dst address
	call read_sector
	
	push 0x8000
	call print
	
	
	; push log_ldkernel
	; call print
	; Load C kernel to RAM
	
	; push log_done
	; call print
	; Jump to kernel

	cli
	hlt
	
; disk parameters
disk: db 0x0
num_heads: dw 0x0000
sectors_per_track: dw 0x0000

; subroutines
on_error:
	push err_diskio
	call print
	cli
	hlt
		
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
	jc on_error ; carry flag means error
	
	
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
log_readfat: db "Reading FAT data...", 0xa, 0xd, 0
log_ldkernel: db "Loading kernel...", 0x0a, 0x0d, 0
log_done: db "Entering kernel... bye :3", 0x0a, 0x0d, 0

; Boot loader error messages
err_diskio: db "Disk IO Error!", 0xa, 0xd, 0

; Other strings
kernel_file: db "NEKOLD", 0