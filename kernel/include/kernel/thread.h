#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/registers.h>
#define THREAD_STACK_SIZE 4096

namespace Kernel
{
    typedef void (*ThreadMain)(void);

    class Thread
    {
    private:
        void *stack;

        static int idCounter;

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