bits 16    ; 16-bit real mode
org 0x7C00 ; BIOS offset

boot:
	xor ax, ax ; Zero registers
	mov ds, ax
	mov es, ax
	
	mov ax, 0x07E0 	 ; let's make a stack (8k in size)
	mov ss, ax
	mov sp, 0x2000
	
	mov [disk], dl ; save our disk

	call clearscreen ; clear the screen... duh!
	
	push headermsg ; log our beautiful OS name!
	call print
	
	; push 0x7C00+0x1BE ; load offset of the FAT partition table
	; call print

	; load a file from disk:
	; mov ah, 0x2 ;read sector
	; mov al, 1 ;1 sector
	; mov ch, 0 ; cyl
	; mov dh, 0 ; head
	; mov cl, 3 ; sector idx
	; mov dl, [disk] ; disk
	; mov bx, 0x7000 ;dst ptr
	; int 0x13
	
	; and put that file content on the screen
	; push 0x7000
	; call print
	
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

disk: db 0x0
headermsg: db "nekosys Bootloader",0xa,0xd,0

times 446 - ($-$$) db 0 ; pad with 0s until the 1st partition table entry at byte 446

db 0x80			 ; 1st partition table entry
times 15 db 0

db 0x00			 ; 2nd partition table entry
times 15 db 0

db 0x00			 ; 3rd partition table entry
times 15 db 0

db 0x00			 ; 4th partition table entry
times 15 db 0

dw 0xaa55 ; boot signature