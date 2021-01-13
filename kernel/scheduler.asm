%include "kernel/arch/regutil.asm"

extern scheduler_preempt_current

global scheduler_yield
scheduler_yield:
    save_all_regs
    push s_eax ; push address of first on stack
    call scheduler_preempt_current
    load_all_regs
    ret