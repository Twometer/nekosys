#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/stack.h>
#include <kernel/registers.h>
#define THREAD_STACK_SIZE 4096

namespace Kernel
{
    typedef void (*ThreadMain)(void);

    class Thread
    {
    private:
        static int idCounter;

        Stack stack;

    public:
        static Thread *current;

        int id;
        uint32_t unblock_time = 0;
        uint32_t run_start_time = 0;
        RegisterStates registers{};
        bool yielded;

        ThreadMain entryPoint;

        Thread(ThreadMain entryPoint);

        void Start();

        void Sleep(int ms);

        void Yield();

        uint32_t GetRuntime();
    };

}; // namespace Kernel

#endif