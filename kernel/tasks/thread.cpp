#include <stdlib.h>
#include <string.h>
#include <kernel/timemanager.h>
#include <kernel/tasks/thread.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/memory/pagemanager.h>
#include <sys/syscall.h>

using namespace Memory;

// FIXME: All blocking methods should not wait for the
//        timer interrupt but pass control to the scheduler
//        instantly

namespace Kernel
{
    Thread *Thread::current = nullptr;
    int Thread::idCounter = 0;

    static void kernel_thread_exit()
    {
        auto thread = Thread::Current();
        thread->SetState(ThreadState::Dead);
        printf("Kernel thread %x died\n", thread->GetId());

        // wait for the scheduler to get us out of here
        Interrupts::WaitForInt();
    }

    Thread::Thread(PageDirectory *pagedir, Stack *stack, Ring ring)
        : id(idCounter++), pagedir(pagedir), stack(stack), ring(ring)
    {
    }

    Thread::~Thread()
    {
        // TODO
    }

    Thread *Thread::CreateKernelThread(ThreadMain entryPoint)
    {
        auto stack = new Stack(new uint8_t[THREAD_STACK_SIZE], THREAD_STACK_SIZE);
        stack->Push((uint32_t)kernel_thread_exit); // Kernel thread return func
        stack->Push(0x202);                        // Flags: Interrupts enabled
        stack->Push(SEG_KRNL_CODE);                // Code segment
        stack->Push((uint32_t)entryPoint);         // IP = entry point

        auto thread = new Thread(PageDirectory::GetKernelDir(), stack, Ring::Ring0);

        auto &regs = thread->GetRegisters();
        regs.ds = SEG_KRNL_DATA;
        regs.esp = (uint32_t)stack->GetStackPtr();

        return thread;
    }

    Thread *Thread::CreateUserThread(ThreadMain entryPoint, Memory::PageDirectory *pagedir, Stack *stack)
    {
        stack->Push(SEG_USER_DATA | 0b11);           // Stack segment
        stack->Push((uint32_t)stack->GetStackPtr()); // Stack pointer
        stack->Push(0x202);                          // Flags: Interrupts enabled
        stack->Push(SEG_USER_CODE | 0b11);           // Code segment
        stack->Push((uint32_t)entryPoint);           // IP = entry point

        auto thread = new Thread(pagedir, stack, Ring::Ring3);

        auto &regs = thread->GetRegisters();
        regs.ds = SEG_USER_DATA;
        regs.esp = (uint32_t)stack->GetStackPtr();

        return thread;
    }

    Thread *Thread::CreateDummyThread()
    {
        auto thread = new Thread(nullptr, nullptr, Ring::Ring0);
        thread->SetState(ThreadState::Yielded);
        return thread;
    }

    void Thread::Start()
    {
        Scheduler::GetInstance()->Start(this);
    }

    void Thread::Sleep(int ms)
    {
        unblockTime = TimeManager::GetInstance()->GetUptime() + ms;
        Yield();
    }

    void Thread::Yield()
    {
        if (!IsCurrent())
            return;

        threadState = ThreadState::Yielded;
        Interrupts::WaitForInt();
    }

    void Thread::Kill()
    {
        if (!IsCurrent())
            return;

        // just set our state to dead and wait for the scheduler to annihilate us
        threadState = ThreadState::Dead;
        Interrupts::WaitForInt();
    }

    uint32_t Thread::GetRuntime()
    {
        return TimeManager::GetInstance()->GetUptime() - currentSliceStart;
    }

    void Thread::BeginSlice()
    {
        currentSliceStart = TimeManager::GetInstance()->GetUptime();
    }

}; // namespace Kernel