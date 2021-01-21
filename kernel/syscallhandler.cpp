#include <kernel/syscallhandler.h>
#include <kernel/memory/stack.h>
#include <kernel/tasks/thread.h>

using namespace Memory;

namespace Kernel
{
    DEFINE_SINGLETON(SyscallHandler)

    uint32_t sys$$exit(void *param)
    {
        printf("Process exited\n");
        return 0;
    }

    uint32_t sys$$print(void *param)
    {
        printf("%s", param);
        return 0;
    }

    uint32_t sys$$texit(void *param)
    {
        uint32_t exit_code = *(uint32_t *)(param);
        printf("UserThread %d exited with exit code %d\n", Thread::Current()->GetId(), exit_code);
        Thread::Current()->Kill();
        return 0;
    }

    SyscallHandler::SyscallHandler()
    {
        syscalls = new nk::Vector<syscall_t>();
    }

    void SyscallHandler::Register()
    {
        AddSyscall(SYS_TEXIT, sys$$texit);
        AddSyscall(SYS_PRINT, sys$$print);
        AddSyscall(SYS_EXIT, sys$$exit);
        Interrupts::AddHandler(0x80, this);
    }

    void SyscallHandler::HandleInterrupt(unsigned int, RegisterStates *regs)
    {
        // When the interrupt is called, ESP is saved to the regs state.
        // At that point however, it is already the kernel's ESP0. The
        // user-mode stack pointer was pushed to the stack by the CPU.
        // The value at ESP+12 therefore gets us the address of where
        // the user thread's stack is located. There, the syscall params
        // are stored.
        auto user_stack_ptr = *(uint32_t *)(regs->esp + 12);

        // For easy access to the struct, we create ourselves a "virtual"
        // stack
        Stack stack((void *)user_stack_ptr);
        stack.Pop();                                        // The return pointer - we don't need that
        uint32_t syscall_num = stack.Pop();                 // Syscall number
        void *param_ptr = (void *)stack.Pop();              // Pointer to the param struct
        uint32_t *retval = (uint32_t *)stack.GetStackPtr(); // Stack now points at the retval ptr
        auto syscall_ptr = syscalls->At(syscall_num);
        if (syscall_ptr == nullptr)
        {
            *retval = -1;
            printf("warn: Thread %d attempted invalid syscall %x\n", Thread::Current()->GetId(), syscall_num);
            return;
        }
        else
        {
            *retval = syscall_ptr(param_ptr);
        }
    }

    void SyscallHandler::AddSyscall(uint32_t number, syscall_t call)
    {
        syscalls->Reserve(number + 1);
        syscalls->At(number) = call;
    }

} // namespace Kernel