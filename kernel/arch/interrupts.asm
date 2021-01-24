; Total number of interrupt vectors
%define NUM_VECTORS 16

%include "kernel/arch/regutil.asm"

; define ptr to register states struct
register_states equ s_eax
global register_states

; Interrupt handler generator macro
%macro def_ireq_handler 1
global irq%1
extern irq%1_handler

irq%1:
  ; save regs
  save_all_regs
  push dword eax
  push dword ebx
  push dword ecx
  push dword edx
  push dword esi
  push dword edi
  push dword ebp
  ; save data segment
  push word ds
  
  ; call the interrupt handler
  call irq%1_handler

  ; load data segment(s)
  pop word ds
  mov ax, ds
  mov es, ax 
  mov fs, ax 
  mov gs, ax
  ; restore regs
  pop dword ebp
  pop dword edi
  pop dword esi
  pop dword edx
  pop dword ecx
  pop dword ebx
  pop dword eax
  iret
%endmacro

; Call the handler generator for each vector
%assign vector 0
%rep NUM_VECTORS
def_ireq_handler vector
%assign vector vector + 1
%endrep

; Define the syscall handler
def_ireq_handler 128

; IDT loader code
global load_idt

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	ret