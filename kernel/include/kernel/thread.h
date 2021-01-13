#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/stack.h>
#include <kernel/registers.h>
#define THREAD_STACK_SIZE 4096

namespace Kernel
{
    typedef void (*ThreadMain)(void);

    enum class ThreadState
    {
        Ready,
        Running,
        Blocked
    };

    class Thread
    {
    private:
        static int idCounter;

        Stack stack;

    public:
        static Thread *current;

        int id;
        uint32_t unblock_time = 0;
        ThreadState thread_state = ThreadState::Ready;
        RegisterStates registers;

        ThreadMain entryPoint;

        Thread(ThreadMain entryPoint);

        void Sleep(int ms);
    };

}; // namespace Kernel

#endif