#ifndef _THREAD_H
#define _THREAD_H

#define THREAD_STACK_SIZE 4096

namespace Kernel
{
    typedef void (*ThreadMain)(void);

    class Thread
    {
    private:
        void *stack;

    public:
        static Thread *current;

        ThreadMain entryPoint;

        Thread(ThreadMain entryPoint);

        void Sleep(int ms);
    };

}; // namespace Kernel

#endif