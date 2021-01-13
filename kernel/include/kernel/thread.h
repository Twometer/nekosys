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

        RegisterStates registers;

        ThreadMain entryPoint;

        Thread(ThreadMain entryPoint);

        void Sleep(int ms);
    };

}; // namespace Kernel

#endif