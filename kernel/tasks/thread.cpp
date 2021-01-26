#include <stdlib.h>
#include <string.h>
#include <kernel/timemanager.h>
#include <kernel/tasks/thread.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/blockers.h>
#include <kernel/memory/pagemanager.h>
#include <sys/syscall.h>

using namespace Memory;

namespace Kernel
{
    Thread *Thread::current = nullptr;
    tid_t Thread::idCounter = 0;

    static void kernel_thread_exit()
    {
        auto thread = Thread::Current();
        thread->SetState(ThreadState::Dead);
        printf("Kernel thread %x died\n", thread->GetId());

        Scheduler::GetInstance()->Yield();
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

    Thread *Thread::CreateKernelThread(ThreadMain entryPoint)
    {
        auto stack = new Stack(new uint8_t[THREAD_STACK_SIZE], THREAD_STACK_SIZE);
        stack->Push((uint32_t)kernel_thread_exit); // Kernel thread return func
        stack->Push(0x202);                        // Flags: Interrupts enabled
        stack->Push(SEG_KRNL_CODE);                // Code segment
        stack->Push((uint32_t)entryPoint);         // IP = entry point
        stack->Push((uint32_t)threadTrampoline);

        auto thread = new Thread(PageDirectory::GetKernelDir(), stack, Ring::Ring0);
        thread->esp = (uint32_t)stack->GetStackPtr();

        return thread;
    }

    Thread *Thread::CreateUserThread(ThreadMain entryPoint, Memory::PageDirectory *pagedir, Stack *stack)
    {
        stack->Push(SEG_USER_DATA | 0b11);           // Stack segment
        stack->Push((uint32_t)stack->GetStackPtr()); // Stack pointer
        stack->Push(0x202);                          // Flags: Interrupts enabled
        stack->Push(SEG_USER_CODE | 0b11);           // Code segment
        stack->Push((uint32_t)entryPoint);           // IP = entry point
        stack->Push((uint32_t)threadTrampoline);

        auto thread = new Thread(pagedir, stack, Ring::Ring3);
        thread->kernelStack = (uint32_t)(new uint8_t[THREAD_STACK_SIZE]) + THREAD_STACK_SIZE;
        thread->esp = (uint32_t)stack->GetStackPtr();

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
        Scheduler::GetInstance()->Yield();
    }

    void Thread::Kill()
    {
        // just set our state to dead and yield away
        threadState = ThreadState::Dead;

        if (!IsCurrent())
            return;

        Scheduler::GetInstance()->Yield();
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