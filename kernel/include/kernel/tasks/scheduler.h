#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <kernel/interrupts.h>
#include <kernel/tasks/thread.h>
#include <nk/vector.h>
#include <nk/singleton.h>

namespace Kernel
{

    class Scheduler : public InterruptHandler
    {
        DECLARE_SINGLETON(Scheduler)

    private:
        nk::Vector<Thread *> threads;

        int ctr = 0;

    public:
        void Initialize();

        void Start(Thread *thread);

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

        void Yield();

    private:
        void PreemptCurrent();

        void ContextSwitch(Thread *oldThread, Thread *newThread);

        Thread *FindNextThread();

        bool CanRun(Thread *thread);

        __attribute__((always_inline)) static inline uint32_t GetEsp()
        {
            uint32_t esp = 0;
            asm volatile("mov %%esp, %0"
                         : "=r"(esp));
            return esp;
        }

        __attribute__((always_inline)) static inline void SetEsp(uint32_t esp)
        {
            asm volatile("mov %0, %%esp"
                         :
                         : "r"(esp));
        }
    };

}; // namespace Kernel

#endif