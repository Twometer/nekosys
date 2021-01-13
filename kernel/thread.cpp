#include <stdlib.h>
#include <string.h>
#include <kernel/thread.h>

namespace Kernel
{

    Thread *Thread::current = nullptr;
    int Thread::idCounter = 0;

    Thread::Thread(ThreadMain entryPoint)
    {
        this->id = idCounter++;
        this->entryPoint = entryPoint;

        // allocate stack
        this->stack = malloc(THREAD_STACK_SIZE) + THREAD_STACK_SIZE;

        // put the instruction pointer on the stack
        size_t ipSize = sizeof(entryPoint);
        memcpy(this->stack - ipSize, &entryPoint, sizeof(entryPoint));
        stack = stack - ipSize;

        // set stack pointer
        registers.esp = (uint32_t)this->stack;
    }

    void Thread::Sleep(int ms)
    {
    }

}; // namespace Kernel