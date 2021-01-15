global enable_paging
enable_paging:
    push eax
    mov eax, cr0
    or eax, 0x80000000 ; set paging bit
    mov cr0, eax
    pop eax
    ret

global load_page_directory
load_page_directory:
    push eax
    mov eax, [esp + 8]
    mov cr3, eax
    pop eax
    ret