#include <stdlib.h>
#include <stdio.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/timemanager.h>

#define SCHEDULER_DBG 0

namespace Kernel
{
    DEFINE_SINGLETON(Scheduler)

    Scheduler::Scheduler()
    {
    }

    void Scheduler::Initialize()
    {
        tss = malloc(1024);
        memset(tss, 0x00, 1024);
        Thread::current = new Thread(nullptr);
        Thread::current->yielded = true;
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

    void Scheduler::HandleInterrupt(unsigned int, RegisterStates *regs)
    {
        auto *currentThread = Thread::current;
        if (currentThread->yielded || currentThread->GetRuntime() > 25)
        {
            currentThread->yielded = false;
            PreemptCurrent(regs);
        }
    }

    void Scheduler::PreemptCurrent(RegisterStates *regs)
    {
        if (threads.Size() == 0)
            return;

        auto oldThread = Thread::current;
        auto newThread = FindNextThread();
        ContextSwitch(oldThread, newThread, regs);
    }

    void Scheduler::ContextSwitch(Thread *oldThread, Thread *newThread, RegisterStates *regs)
    {
        if (newThread == oldThread || newThread->id == oldThread->id)
        {
            // well...
            return;
        }

        // set thread as current
        Thread::current = newThread;
        oldThread->last_cpu_time = oldThread->GetRuntime();
        newThread->run_start_time = TimeManager::GetInstance()->GetUptime();

// context switch:
#if SCHEDULER_DBG
        if (oldThread->id != 0)
        {
            auto &newregs = newThread->registers;
            printf("scheduler: %d -> %d after %dms\n", oldThread->id, newThread->id, oldThread->GetRuntime());
            printf("  old: %x, %x\n", regs->esp, regs->eax);
            printf("  new: %x, %x\n", newregs.esp, newregs.eax);
        }
#endif

        // save current regs to old thread
        regs->CopyTo(&oldThread->registers);

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
        return thread != nullptr && thread->unblock_time <= TimeManager::GetInstance()->GetUptime() && !thread->yielded;
    }

}; // namespace Kernel