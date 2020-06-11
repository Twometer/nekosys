bits 16    ; 16-bit real mode
org 0x7C00 ; BIOS offset

boot:
	xor ax, ax ; Zero registers
	mov ds, ax
	mov es, ax
	
	mov ax, 0x07E0 	 ; let's make a stack (8k in size)
	mov ss, ax
	mov sp, 0x2000

	call clearscreen ; clear the screen... duh!
	
	push headermsg ; log our beautiful OS name!
	call print
	
	push bootmsg ; log more info because it looks cool
	call print
	
	cli ; clear interrupts
	hlt ; halt execution

	
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
	
	mov ah, 0x07        ; tells BIOS to scroll down window
	mov al, 0x00        ; clear entire window
	mov bh, 0x07        ; white on black
	mov cx, 0x00        ; specifies top left of screen as (0,0)
	mov dh, 0x20        ; 18h = 24 rows of chars
	mov dl, 0x50        ; 4fh = 79 cols of chars
	int 0x10            ; calls video interrupt
	
	popa
	
	mov sp, bp ;return stack frame
    pop bp
	
	ret

headermsg: db "nekosys Bootloader",0xa,0xd,0
bootmsg: db "Loading kernel...",0xa,0xd,0

times 446 - ($-$$) db 0 ; pad with 0s until the 1st partition table entry at byte 446

; TODO: Bios complains about invalid partition table
db 0x01			 ; 1st partition table entry
times 15 db 0

db 0x01			 ; 2nd partition table entry
times 15 db 0

db 0x01			 ; 3rd partition table entry
times 15 db 0

db 0x01			 ; 4th partition table entry
times 15 db 0

dw 0xaa55 ; boot signature