bits 16		; still 16 bit
org 0x7F00  ; loader offset

init:
	call clearscreen
	
	push log_welcome
	call print
	
	push log_readfat
	call print
	; Read the FAT system
	
	; push log_ldkernel
	; call print
	; Load C kernel to RAM
	
	; push log_done
	; call print
	; Jump to kernel

	cli
	hlt
	
print:
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
		
	ret
	
clearscreen:
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
	
	ret
	
log_welcome: db "nekosys Bootloader", 0xa, 0xd, 0
log_readfat: db "Reading FAT data...", 0xa, 0xd, 0
log_ldkernel: db "Loading kernel...", 0x0a, 0x0d, 0
log_done: db "Entering kernel... bye :3", 0x0a, 0x0d, 0