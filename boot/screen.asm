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

;;;;;;;;;;;;;;;;;;;;;;
;; String constants ;;
;;;;;;;;;;;;;;;;;;;;;;
newline: db 0x0a, 0x0d, 0
hextable: db "0123456789ABCDEF"