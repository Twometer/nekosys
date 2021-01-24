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
    tid_t Thread::idCounter = 0;

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
        delete[] stack->GetStackBottom();
        delete stack;

        if (ring == Ring::Ring3)
        {
            auto kernelStackBottom = (uint8_t *)(kernelStack - THREAD_STACK_SIZE);
            delete[] kernelStackBottom;
        }
    }

    extern "C" void threadTrampoline();

    extern "C" void testTramp()
    {
        printf("REACHED TEST TRAMPOLINE\n");
        for(;;);
    }

    Thread *Thread::CreateKernelThread(ThreadMain entryPoint)
    {
        auto stack = new Stack(new uint8_t[THREAD_STACK_SIZE], THREAD_STACK_SIZE);
        stack->Push((uint32_t)kernel_thread_exit); // Kernel thread return func
        stack->Push(0x202);                        // Flags: Interrupts enabled
        stack->Push(SEG_KRNL_CODE);                // Code segment
        stack->Push((uint32_t)entryPoint);         // IP = entry point
        //stack->Push((uint32_t)SEG_KRNL_DATA);
        stack->Push((uint32_t)threadTrampoline);

        auto thread = new Thread(PageDirectory::GetKernelDir(), stack, Ring::Ring0);

        auto &regs = thread->GetRegisters();
        regs.ds = SEG_KRNL_DATA;
        regs.esp = (uint32_t)stack->GetStackPtr();
        thread->esp = regs.esp;

        return thread;
    }

    Thread *Thread::CreateUserThread(ThreadMain entryPoint, Memory::PageDirectory *pagedir, Stack *stack)
    {
        //stack = new Stack(new uint8_t[THREAD_STACK_SIZE], THREAD_STACK_SIZE);
        stack->Push(SEG_USER_DATA | 0b11);           // Stack segment
        stack->Push((uint32_t)stack->GetStackPtr()); // Stack pointer
        stack->Push(0x202);                          // Flags: Interrupts enabled
        stack->Push(SEG_USER_CODE | 0b11);           // Code segment
        stack->Push((uint32_t)entryPoint);           // IP = entry point
        //stack->Push((uint32_t)SEG_USER_DATA);
        stack->Push((uint32_t)threadTrampoline);

        auto thread = new Thread(pagedir, stack, Ring::Ring3);
        thread->kernelStack = (uint32_t)(new uint8_t[THREAD_STACK_SIZE]) + THREAD_STACK_SIZE;

        auto &regs = thread->GetRegisters();
        regs.ds = SEG_USER_DATA;
        regs.esp = (uint32_t)stack->GetStackPtr();
        thread->esp = regs.esp;

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
        Block(new SleepThreadBlocker(ms));
    }

    void Thread::Yield()
    {
        if (!IsCurrent())
            return;

        threadState = ThreadState::Yielded;
        Scheduler::GetInstance()->HandleInterrupt(0, nullptr);
        //Interrupts::WaitForInt();
    }

    void Thread::Kill()
    {
        // just set our state to dead and wait for the scheduler to annihilate us
        threadState = ThreadState::Dead;

        if (!IsCurrent())
            return;

        Interrupts::WaitForInt();
    }

    void Thread::Block(IThreadBlocker *blocker)
    {
        this->blocker = blocker;
        Yield();
    }

    void Thread::Unblock()
    {
        delete blocker;
        blocker = nullptr;
    }

    bool Thread::IsBlocked()
    {
        return blocker != nullptr && blocker->IsBlocked();
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