#include <stdlib.h>
#include <kernel/thread.h>

namespace Kernel
{

    Thread *Thread::current = nullptr;

    Thread::Thread(ThreadMain entryPoint)
    {
        this->entryPoint = entryPoint;
        this->stack = malloc(THREAD_STACK_SIZE) + THREAD_STACK_SIZE;
    }

    void Thread::Sleep(int ms)
    {
    }

}; // namespace Kernel