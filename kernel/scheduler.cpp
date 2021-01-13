#include <stdlib.h>
#include <stdio.h>
#include <kernel/scheduler.h>
#include <kernel/timemanager.h>

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
        if (threads.Size() == 0)
            return;

        auto newThread = FindNextThread();
        auto oldThread = Thread::current;

        if (newThread == oldThread)
        {
            // well...
            return;
        }

        // set thread as current
        Thread::current = newThread;

// context switch:
#if SCHEDULER_DBG
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

    Thread *Scheduler::FindNextThread()
    {
        // TODO: This is a shitty scheduler function
        Thread *nextThread = nullptr;
        do
        {
            int nextIdx = (ctr++) % threads.Size();
            nextThread = threads.At(nextIdx);
        } while (!CanRun(nextThread));
        return nextThread;
    }

    bool Scheduler::CanRun(Thread *thread)
    {
        return thread != nullptr && thread->unblock_time <= TimeManager::get_instance()->get_uptime();
    }

}; // namespace Kernel