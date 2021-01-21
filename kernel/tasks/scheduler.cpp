#include <stdlib.h>
#include <stdio.h>
#include <kernel/panic.h>
#include <kernel/timemanager.h>
#include <tasks/scheduler.h>

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

        // The dummy thread is the "thread" that
        // did execution before the scheduler managed it.
        // We define that here for easier handling below.
        auto dummyThread = Thread::CreateDummyThread();
        dummyThread->MakeCurrent();

        // Register timer interrupt
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
        auto *currentThread = Thread::Current();
        if (currentThread->GetState() != ThreadState::Running || currentThread->GetRuntime() > 25)
        {
            if (currentThread->GetState() != ThreadState::Dead)
            {
                // if it isn't dead, set it to runnable
                currentThread->SetState(ThreadState::Runnable);
            }
            else
            {
                // remove dead threads
                for (size_t i = 0; i < threads.Size(); i++)
                {
                    if (threads.At(i)->GetId() == currentThread->GetId())
                    {
                        threads.Remove(i);
                        break;
                    }
                }
            }
            PreemptCurrent(regs);
        }
    }

    void Scheduler::PreemptCurrent(RegisterStates *regs)
    {
        if (threads.Size() == 0)
            Kernel::Panic("scheduler", "Who the fuck killed the idle process?");

        auto oldThread = Thread::Current();
        auto newThread = FindNextThread();
        ContextSwitch(oldThread, newThread, regs);
    }

    void Scheduler::ContextSwitch(Thread *oldThread, Thread *newThread, RegisterStates *regs)
    {
        if (newThread == oldThread || newThread->GetId() == oldThread->GetId())
        {
            // well...
            return;
        }

        // Set new thread as current
        newThread->MakeCurrent();
        newThread->SetState(ThreadState::Running);
        newThread->BeginSlice();

// context switch:
#if SCHEDULER_DBG
        auto &newregs = newThread->GetRegisters();
        printf("scheduler: %d -> %d after %dms\n", oldThread->GetId(), newThread->GetId(), oldThread->GetRuntime());
        printf("  old: %x, %x\n", regs->esp, regs->ds);
        printf("  new: %x, %x\n", newregs.esp, newregs.ds);
#endif

        // Save current regs to old thread
        regs->CopyTo(&oldThread->GetRegisters());

        // Load regs for next thread
        newThread->GetRegisters().CopyTo(regs);

        // Load pages for new thread
        if (!newThread->GetPageDir()->IsCurrent())
        {
#if SCHEDULER_DBG
            printf("scheduler: loaded new page table\n");
#endif
            newThread->GetPageDir()->Load();
        }
    }

    Thread *Scheduler::FindNextThread()
    {
        // FIXME: This is a shitty scheduler function
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
        return thread != nullptr && thread->GetUnblockTime() <= TimeManager::GetInstance()->GetUptime() && thread->GetState() == ThreadState::Runnable;
    }

}; // namespace Kernel