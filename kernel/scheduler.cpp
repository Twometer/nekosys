#include <stdlib.h>
#include <kernel/scheduler.h>

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
        SwitchToTask(threads.At(nextTaskIdx));
    }

    void Scheduler::SwitchToTask(Thread *thread)
    {
        Thread::current = thread;

        // todo we need register states:
        //  when the interrupt is fired, save all registers to a struct in memory
        //  when the interrupt returns, restore the regs from that struct back into the CPU
        //  result: we can easily modify any registers, and only have them apply once the ISR finishes
        //  benefit: for a new task, we can build a stack that just has its entry point instr pointer pushed to it
        //  -> all the registers must therefore be saved in the thread

    }

}; // namespace Kernel