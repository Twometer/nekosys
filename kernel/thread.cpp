#include <stdlib.h>
#include <string.h>
#include <kernel/thread.h>
#include <kernel/timemanager.h>

namespace Kernel
{

    Thread *Thread::current = nullptr;
    int Thread::idCounter = 0;

    Thread::Thread(ThreadMain entryPoint) : stack(THREAD_STACK_SIZE)
    {
        this->id = idCounter++;
        this->entryPoint = entryPoint;

        // put stuff on stack that iret needs later
        // note for the flags: I observed this value from the call stack in an irq.
        //                     I know that it keeps interrupts enabled. I don't know what else it does, but it works.
        
        stack.push(0x202);                // Flags, 0x202 for now.
        stack.push(0x08);                 // CS = 0x08
        stack.push((uint32_t)entryPoint); // IP = entry_point

        // set stack pointer
        registers.esp = (uint32_t)this->stack.get_stack_ptr();
    }

    void Thread::Sleep(int ms)
    {
        unblock_time = TimeManager::get_instance()->get_uptime() + ms;
        thread_state = ThreadState::Blocked;
        asm("hlt"); // TODO: don't wait for the interrupt, but yield instantly...?
    }

}; // namespace Kernel