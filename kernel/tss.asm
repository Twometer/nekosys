global setTss

setTss:
    mov ax, [esp + 4] ; tss address is first argument
    ltr ax
    ret