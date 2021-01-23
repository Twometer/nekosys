#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/ring.h>
#include <kernel/registers.h>
#include <kernel/memory/stack.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/tasks/process.h>

#define THREAD_STACK_SIZE 4096

namespace Kernel
{
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
        static int idCounter;

        int id;
        Ring ring;
        RegisterStates registers{};
        ThreadState threadState = ThreadState::Runnable;

        Memory::PageDirectory *pagedir;
        Memory::Stack *stack = nullptr;

        Process *process;

        uint32_t currentSliceStart = 0;
        uint32_t unblockTime = 0;

    private:
        Thread(Memory::PageDirectory *pagedir, Memory::Stack *stack, Ring ring);

    public:
        ~Thread();

        static Thread *CreateKernelThread(ThreadMain entryPoint);

        static Thread *CreateUserThread(ThreadMain entryPoint, Memory::PageDirectory *pagedir, Memory::Stack *stack);

        static Thread *CreateDummyThread();

        void Start();

        void Sleep(int ms);

        void Yield();

        void Kill();

        int GetId() { return id; }

        bool IsCurrent() { return current == this; };
        void MakeCurrent() { current = this; }

        ThreadState GetState() { return threadState; }
        void SetState(ThreadState state) { threadState = state; }

        Process *GetProcess() { return process; };
        void SetProcess(Process *process) { this->process = process; }

        uint32_t GetRuntime();

        uint32_t GetUnblockTime() { return unblockTime; }

        Memory::PageDirectory *GetPageDir() { return pagedir; }

        Ring GetRing() { return ring; }

        RegisterStates &GetRegisters() { return registers; }

        void BeginSlice();

        static Thread *Current() { return current; }
    };

}; // namespace Kernel

#endif