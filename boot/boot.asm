bits 16    ; 16-bit
org 0x7C00 ; BIOS offset

; entry point 16 bit real mode
boot:
	xor ax, ax ; Zero registers
	mov ds, ax ; data segment at zero
	mov es, ax ; extra segment at zero
	
	mov ax, 0x07E0 	 ; let's make a stack (8k in size)
	mov ss, ax
	mov sp, 0x2000
	
	mov [disk], dl ; save our disk

	mov ah, 0x2	; op
	mov al, 0x1 ; sector count
	mov ch, 0x0 ; cyl
	mov cl, 0x2 ; sector
	mov dh, 0x0 ; head
	mov bx, 0x7F00 ; put it at 0x7f00
	mov dl, [disk] ;diskno
	int 0x13
	
	jc _error ; if carry is set: instant error
	cmp ah, 0
	je read  ; if ah is zero, read
	
	_error:		; write an error message
	push ioerr
	call print
	
	cli ; and halt
	hlt
	
	read:	; read was ok, log that
	push iogood
	call print
	
	jmp 0x7F00 ; and transfer execution to the big chungus loader
	
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

; messages
ioerr: db "Failed to access boot disk", 0xa,0xd,0
iogood: db "Bootloader loaded",0xa,0xd,0

; global descriptor table

gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:
gdt_pointer:
    dw gdt_end - gdt_start
    dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

	

	
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

; 32-bit code:
bits 32
boot32:
	; init registers 
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esi,hello
	mov ebx,0xb8000
.loop:
	lodsb
	or al,al
	jz halt
	or eax,0x0100
	mov word [ebx], ax
	add ebx,2
	jmp .loop
halt:
	cli
	hlt
hello: db "nekosys Bootloader",0



times 510 - ($-$$) db 0 ; pad to full sector
dw 0xaa55 ; boot signature