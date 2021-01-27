#include <stdlib.h>
#include <stdio.h>
#include <kernel/panic.h>
#include <kernel/timemanager.h>
#include <kernel/tasks/process.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/kdebug.h>
#include <kernel/gdt.h>

namespace Kernel
{
    DEFINE_SINGLETON(Scheduler)

    Scheduler::Scheduler()
    {
    }

    void Scheduler::Initialize()
    {
        // The dummy thread is the "thread" that
        // did execution before the scheduler managed it.
        // We define that here for easier handling below.
        auto dummyThread = Thread::CreateDummyThread();
        dummyThread->MakeCurrent();

        // Register timer interrupt
        Interrupts::AddHandler(0x00, this);
    }

    void Scheduler::Start(Thread *thread)
    {
        threads.Add(thread);
    }

    void Scheduler::Yield()
    {
        // to yield, just simulate the timer interrupt
        HandleInterrupt(0, nullptr);
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
            PreemptCurrent();
        }
    }

    void Scheduler::PreemptCurrent()
    {
        if (threads.Size() == 0)
            Kernel::Panic("scheduler", "Who the fuck killed the idle process?");

        auto oldThread = Thread::Current();
        auto newThread = FindNextThread();
        ContextSwitch(oldThread, newThread);
    }

    extern "C" void changeStack(uint32_t *oldEsp, uint32_t newEsp);

    void Scheduler::ContextSwitch(Thread *oldThread, Thread *newThread)
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
        newThread->Unblock();

        Process::SetCurrent(newThread->GetProcess());

// context switch:
#if SCHEDULER_DEBUG
        kdbg("scheduler: %d -> %d after %dms\n", oldThread->GetId(), newThread->GetId(), oldThread->GetRuntime());
        kdbg("  old: %x\n", GetEsp());
        kdbg("  new: %x\n", newThread->esp);
#endif

        // Load new kernel stack pointer for that thread.
        GDT::GetInstance()->GetTssEntry().esp0 = newThread->GetKernelStack();

        // Destroy dead threads
        if (oldThread->GetState() == ThreadState::Dead)
        {
#if SCHEDULER_DEBUG
            kdbg("scheduler: Annihilating dead thread %d\n", oldThread->GetId());
#endif
            delete oldThread;
        }

        // Load pages for new thread
        if (!newThread->GetPageDir()->IsCurrent())
        {
            newThread->GetPageDir()->Load();
#if SCHEDULER_DEBUG
            kdbg("scheduler: loaded new page table\n");
#endif
        }

        // Change stack pointers
        changeStack(&oldThread->esp, newThread->esp);
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
        return thread != nullptr && !thread->IsBlocked() && thread->GetState() == ThreadState::Runnable;
    }

}; // namespace Kernel