#ifndef _SYSCALLHANDLER_H
#define _SYSCALLHANDLER_H

#include <nk/singleton.h>
#include <nk/vector.h>
#include <sys/syscall.h>
#include <kernel/arch/interrupts.h>

namespace Kernel
{
    typedef int (*syscall_t)(void *);

    class SyscallHandler : public InterruptHandler
    {
        DECLARE_SINGLETON(SyscallHandler)
    private:
        nk::Vector<syscall_t> *syscalls;

    public:
        void Register();

    private:
        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

        void AddSyscall(uint32_t number, syscall_t call);
    };

}; // namespace Kernel

#endif