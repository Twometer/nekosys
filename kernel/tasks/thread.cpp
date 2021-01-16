#include <stdlib.h>
#include <string.h>
#include <kernel/timemanager.h>
#include <tasks/thread.h>
#include <tasks/scheduler.h>

namespace Kernel
{

    Thread *Thread::current = nullptr;
    int Thread::idCounter = 0;

    static void thread_exit_func()
    {
        auto curThread = Thread::current;
        printf("Thread %x died\n", curThread->id);
        curThread->threadState = ThreadState::Dead;
        asm("hlt"); // wait for the scheduler to get us out of here
    }

    Thread::Thread(ThreadMain entryPoint, ThreadLevel level) : stack(THREAD_STACK_SIZE)
    {
        this->id = idCounter++;
        this->entryPoint = entryPoint;

        // put stuff on stack that iret needs later
        // note for the flags: I observed this value from the call stack in an irq.
        //                     I know that it keeps interrupts enabled. I don't know what else it does, but it works.
        stack.push((uint32_t)(thread_exit_func)); // The address to return to after the thread has exited
        stack.push(0x202);                        // Flags, 0x202 for now.
        stack.push(0x08);                         // CS = 0x08
        stack.push((uint32_t)entryPoint);         // IP = entry_point

        if (level == ThreadLevel::Kernel)
        {
            pagedir = Memory::PageDirectory::kernelDir;
        }
        else
        {
            // TODO create userspace dir based on kernel directory
        }

        // set stack pointer
        registers.esp = (uint32_t)this->stack.get_stack_ptr();
    }

    Thread::~Thread()
    {
        printf("thread dying\n");
    }

    void Thread::Start()
    {
        Scheduler::GetInstance()->Start(this);
    }

    void Thread::Sleep(int ms)
    {
        unblock_time = TimeManager::GetInstance()->GetUptime() + ms;
        Yield();
    }

    void Thread::Yield()
    {
        // TODO: don't wait for the interrupt, but yield instantly...?
        threadState = ThreadState::Yielded;
        asm("hlt");
    }

    void Thread::Kill()
    {
        // just set our state to dead and wait for the scheduler to annihilate us
        threadState = ThreadState::Dead;
        asm("hlt");
    }

    uint32_t Thread::GetRuntime()
    {
        return TimeManager::GetInstance()->GetUptime() - run_start_time;
    }

}; // namespace Kernel