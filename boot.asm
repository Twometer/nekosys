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

	call clearscreen ; clear the screen... duh!
	
	push headermsg ; log our beautiful OS name!
	call print
	
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
	
	_error:
	push ioerr
	call print
	
	cli ; exit after error!
	hlt
	
	read:
	push iogood
	call print
	
	push 0x7F00
	call print
	
	; read from disk!
	
	; Switch to 32-bit mode
	; mov ax, 2401
	; int 0x15
	
	; set vga text mode 0x3
	; mov ax, 0x3
	; int 0x10
	
	; load global descriptor table
	; lgdt [gdt_pointer]
	; mov eax, cr0
	; or eax, 0x1 ; set protected mode bit on special regiter cr0
	; mov cr0, eax
	
	; Jump to the 32-bit boot code
	; jmp CODE_SEG:boot32
	
	
	cli ; clear interrupts
	hlt ; halt execution

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
ioerr: db "disk error", 0xa,0xd,0
iogood: db "io ok",0xa,0xd,0

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



; partition table and boot sig:
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