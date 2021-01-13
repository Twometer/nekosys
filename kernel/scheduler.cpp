#include <stdlib.h>
#include <stdio.h>
#include <kernel/scheduler.h>

#define SCHEDULER_DBG 0

namespace Kernel
{

    void Scheduler::Initialize()
    {
        tss = malloc(1024);
        memset(tss, 0x00, 1024);
        Interrupts::AddHandler(0x00, this);
    }

    void *Scheduler::GetTssPtr()
    {
        return tss;
    }

    void Scheduler::Start(Thread *thread)
    {
        threads.Add(thread);
    }

    void Scheduler::HandleInterrupt(unsigned int interrupt, RegisterStates *regs)
    {
        // TODO: Real scheduling algorithm
        if (threads.Size() == 0)
            return;

        int nextTaskIdx = ctr % threads.Size();
        ctr++;

        auto newThread = threads.At(nextTaskIdx);
        auto oldThread = Thread::current;

        // set thread as current
        Thread::current = newThread;

// context switch:
#if SCHEDULER_DB
        printf("scheduler: Context switch to %d\n", newThread->id);
#endif

        // save current regs to old thread
        if (oldThread != nullptr)
        {
            regs->CopyTo(&oldThread->registers);
        }

        // load regs for next thread
        newThread->registers.CopyTo(regs);
    }

    void Scheduler::SwitchToTask(Thread *newThread)
    {

        // todo we need register states:
        //  when the interrupt is fired, save all registers to a struct in memory
        //  when the interrupt returns, restore the regs from that struct back into the CPU
        //  result: we can easily modify any registers, and only have them apply once the ISR finishes
        //  benefit: for a new task, we can build a stack that just has its entry point instr pointer pushed to it
        //  -> all the registers must therefore be saved in the thread
    }

}; // namespace Kernel