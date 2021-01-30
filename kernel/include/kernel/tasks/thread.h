#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/ring.h>
#include <kernel/arch/registers.h>
#include <kernel/memory/stack.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/tasks/process.h>
#include <kernel/tasks/blocker.h>

#define THREAD_STACK_SIZE 4096

namespace Kernel
{
    typedef uint32_t tid_t;
    typedef void (*ThreadMain)(void);

    enum class ThreadState
    {
        Runnable,
        Running,
        Yielded,
        Dead
    };

    class Process;
    class Thread
    {
    private:
        static Thread *current;
        static tid_t idCounter;

        tid_t id;
        Ring ring;
        ThreadState threadState = ThreadState::Runnable;

        Process *process;

        Memory::PageDirectory *pagedir;
        Memory::Stack *stack = nullptr;
        uint32_t kernelStack = 0;

        IThreadBlocker *blocker = nullptr;
        uint32_t currentSliceStart = 0;

    private:
        Thread(Memory::PageDirectory *pagedir, Memory::Stack *stack, Ring ring);

    public:
        uint32_t esp = 0;

        ~Thread();

        static Thread *CreateKernelThread(ThreadMain entryPoint);

        static Thread *CreateUserThread(ThreadMain entryPoint, Memory::PageDirectory *pagedir, Memory::Stack *stack);

        static Thread *CreateDummyThread();

        void Start();

        void Sleep(int ms);

        void Yield();

        void Kill();

        void Block(IThreadBlocker *blocker);

        void Unblock();

        bool IsBlocked();

        tid_t GetId() { return id; }

        bool IsCurrent() { return current == this; };
        void MakeCurrent() { current = this; }

        ThreadState GetState() { return threadState; }
        void SetState(ThreadState state) { threadState = state; }

        Process *GetProcess() { return process; };
        void SetProcess(Process *process) { this->process = process; }

        uint32_t GetKernelStack() { return kernelStack; }

        uint32_t GetRuntime();

        Memory::PageDirectory *GetPageDir() { return pagedir; }

        Ring GetRing() { return ring; }

        void BeginSlice();

        static Thread *Current() { return current; }
    };

}; // namespace Kernel

#endif