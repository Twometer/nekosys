#include <kernel/syscallhandler.h>
#include <kernel/syscalls.h>
#include <kernel/memory/stack.h>
#include <kernel/tasks/thread.h>
#include <kernel/kdebug.h>
#include <kernel/video/tty.h>

using namespace Memory;

namespace Kernel
{
    DEFINE_SINGLETON(SyscallHandler)

    SyscallHandler::SyscallHandler()
    {
        syscalls = new nk::Vector<syscall_t>();
    }

    void SyscallHandler::Register()
    {
        AddSyscall(SYS_TEXIT, sys$$texit);
        AddSyscall(SYS_PRINT, sys$$print);
        AddSyscall(SYS_EXIT, sys$$exit);
        AddSyscall(SYS_PUTCHAR, sys$$putchar);
        AddSyscall(SYS_FOPEN, sys$$fopen);
        AddSyscall(SYS_FREAD, sys$$fread);
        AddSyscall(SYS_FWRITE, sys$$fwrite);
        AddSyscall(SYS_FCLOSE, sys$$fclose);
        AddSyscall(SYS_PAGEALLOC, sys$$pagealloc);
        AddSyscall(SYS_SLEEP, sys$$sleep);
        AddSyscall(SYS_SPAWNP, sys$$spawnp);
        AddSyscall(SYS_WAITP, sys$$waitp);
        AddSyscall(SYS_READLN, sys$$readln);
        AddSyscall(SYS_FBACQUIRE, sys$$fb_acquire);
        AddSyscall(SYS_FBFLUSH, sys$$fb_flush);
        AddSyscall(SYS_FBRELEASE, sys$$fb_release);
        AddSyscall(SYS_CHDIR, sys$$chdir);
        AddSyscall(SYS_GETCWD, sys$$getcwd);
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
        
        auto syscallHandler = syscalls->At(syscall_num);
        if (syscallHandler == nullptr)
        {
            *retval = -1;
            kdbg("warn: Thread %d attempted invalid syscall %x\n", Thread::Current()->GetId(), syscall_num);
            return;
        }
        else
        {
            *retval = syscallHandler(param_ptr);
        }
    }

    void SyscallHandler::AddSyscall(uint32_t number, syscall_t call)
    {
        syscalls->Reserve(number + 1);
        syscalls->At(number) = call;
    }

} // namespace Kernel
