#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <kernel/interrupts.h>
#include <kernel/thread.h>
#include <nk/vector.h>

namespace Kernel
{

    class Scheduler : public InterruptHandler
    {
    private:
        void *tss;
        nk::Vector<Thread *> threads;
        int ctr = 0;

    public:
        void Initialize();

        void *GetTssPtr();

        void Start(Thread *thread);

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

    private:
        void SwitchToTask(Thread *thread);
    };

}; // namespace Kernel

#endif