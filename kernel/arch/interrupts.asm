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
  ; save reg states
  save_all_regs
  save_segments
  
  ; get the flags from the stack
  ; they are just for analyzing and stuff, and don't get written back
  ;mov dword eax, [esp + 8]
  ;mov dword [s_flags], eax

  ; call interrupt handler
  call irq%1_handler

  ; load reg states back
  load_segments
  load_all_regs
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