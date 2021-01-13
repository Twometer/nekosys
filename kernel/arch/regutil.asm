; Storage for registers during the IRQ
; This must match the RegisterState struct in registers.h
s_eax: dd 0
s_ebx: dd 0
s_ecx: dd 0
s_edx: dd 0
s_esp: dd 0
s_esi: dd 0
s_edi: dd 0
s_flags: dd 0

; Macro for saving register to struct
%macro save_reg 1
mov dword [s_%1], %1
%endmacro

; Macro for loading register from struct
%macro load_reg 1
mov dword %1, [s_%1]
%endmacro

%macro save_all_regs 0
  save_reg eax
  save_reg ebx
  save_reg ecx
  save_reg edx
  save_reg esp
  save_reg esi
  save_reg edi
%endmacro

%macro load_all_regs 0
  load_reg eax
  load_reg ebx
  load_reg ecx
  load_reg edx
  load_reg esp
  load_reg esi
  load_reg edi
%endmacro