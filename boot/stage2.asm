bits 16     ; We're in 16-bit mode
org 0x8000  ; Stage-2 loader offset

;;;;;;;;;;;;;;;;;
;; Entry point ;;
;;;;;;;;;;;;;;;;;
main:
    ; Success message
    push banner
    call print

    cli
    hlt

;;;;;;;;;;;;;;;
;; Libraries ;;
;;;;;;;;;;;;;;;
%include "screen.asm"
%include "disk.asm"

;;;;;;;;;;;;;;;;;;;;;;
;; String constants ;;
;;;;;;;;;;;;;;;;;;;;;;
banner: db "Stage 2 loaded", 0x0a, 0x0d, 0

;;;;;;;;;;;;;
;; Padding ;;
;;;;;;;;;;;;;
times 1024 - ($-$$) db 0 ; Pad to two full sectors