; Total number of interrupt vectors
%define NUM_VECTORS 16

; Storage for registers during the IRQ
; This must match the RegisterState struct in registers.h
s_eax: dd 0
s_ebx: dd 0
s_ecx: dd 0
s_edx: dd 0
s_esp: dd 0
s_esi: dd 0
s_edi: dd 0

register_states equ s_eax
global register_states

; Macro for saving register to struct
%macro save_reg 1
mov dword [s_%1], %1
%endmacro

; Macro for loading register from struct
%macro load_reg 1
mov dword %1, [s_%1]
%endmacro

; Interrupt handler generator macro
%macro def_ireq_handler 1
global irq%1
extern irq%1_handler

irq%1:
  ; save reg states
  save_reg eax
  save_reg ebx
  save_reg ecx
  save_reg edx
  save_reg esp
  save_reg esi
  save_reg edi

  ; call interrupt handler
  call irq%1_handler

  ; load reg states back
  load_reg eax
  load_reg ebx
  load_reg ecx
  load_reg edx
  load_reg esp
  load_reg esi
  load_reg edi
  iret
%endmacro

; Call the handler generator for each vector
%assign vector 0
%rep NUM_VECTORS
def_ireq_handler vector
%assign vector vector + 1
%endrep

; IDT loader code
global load_idt

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	ret