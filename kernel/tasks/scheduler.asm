global threadTrampoline
threadTrampoline:
    mov ax, 0x23
    mov ds, ax
    mov es, ax 
    mov fs, ax 
    mov gs, ax
    ; iret to the thread!
    iret
 
global changeStack
changeStack:
    mov ebx, [esp + 4]
    mov [ebx], esp
    mov esp, [esp + 8]
    
    ret