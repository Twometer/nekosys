#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/ring.h>
#include <kernel/stack.h>
#include <kernel/registers.h>
#include <memory/pagedirectory.h>

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

    class Thread
    {
    private:
        static int idCounter;

        Stack *stack;

    public:
        static Thread *current;

        int id;
        uint32_t unblock_time = 0;
        uint32_t run_start_time = 0;
        uint32_t last_cpu_time = 0;
        RegisterStates registers{};
        Ring ring;
        ThreadState threadState = ThreadState::Runnable;
        Memory::PageDirectory *pagedir;

        ThreadMain entryPoint;

        Thread(ThreadMain entryPoint, Ring ring = Ring::Ring0);

        ~Thread();

        void Start();

        void Sleep(int ms);

        void Yield();

        uint32_t GetRuntime();

        bool IsCurrent() { return current == this; };

        void Kill();
    };

}; // namespace Kernel

#endif