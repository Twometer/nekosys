#include <stdlib.h>
#include <string.h>
#include <kernel/timemanager.h>
#include <tasks/thread.h>
#include <tasks/scheduler.h>
#include <memory/pagemanager.h>

using namespace Memory;

namespace Kernel
{

    Thread *Thread::current = nullptr;
    int Thread::idCounter = 0;

    static void thread_exit_func_krnl()
    {
        auto curThread = Thread::current;
        printf("Thread %x died\n", curThread->id);
        curThread->threadState = ThreadState::Dead;
        asm("hlt"); // wait for the scheduler to get us out of here
    }

    static void thread_exit_func_user()
    {
        // TODO replace with syscall to signal thread exit
        auto curThread = Thread::current;
        printf("Usermode Thread %x died\n", curThread->id);
        curThread->threadState = ThreadState::Dead;
        asm("hlt");
    }

    Thread::Thread(ThreadMain entryPoint, Ring ring)
    {
        this->id = idCounter++;
        this->entryPoint = entryPoint;

        if (ring == Ring::Ring0)
        {
            pagedir = PageDirectory::kernelDir;
            stack = new Stack(new uint8_t[THREAD_STACK_SIZE], THREAD_STACK_SIZE);
            // Push the kernel thread return func#
            stack->Push((uint32_t)(thread_exit_func_krnl));

            // put stuff on stack that an iret needs later
            // note for the flags: I observed this value from the stack in an irq.
            //                     I know that it keeps interrupts enabled. I don't know what else it does, but it works.
            stack->Push(0x202);                // Flags, 0x202 for now.
            stack->Push(SEG_KRNL_CODE);        // Code Segment
            stack->Push((uint32_t)entryPoint); // IP = entry_point
            registers.esp = (uint32_t)this->stack->GetStackPtr();
        }
        else
        {
            pagedir = new PageDirectory(*PageDirectory::kernelDir);

            auto pageframe = PageManager::GetInstance()->AllocPageframe(); // todo save all pages that the thread allocates so that we don't leak memory
            auto stack_virt = (vaddress_t)0x100000;
            pagedir->MapPage(pageframe, stack_virt, PAGE_BIT_READ_WRITE | PAGE_BIT_ALLOW_USER);

            stack = new Stack(stack_virt, THREAD_STACK_SIZE);
            // push the user thread return func
            stack->Push((uint32_t)(thread_exit_func_user));

            // put stuff on the stack that iret needs
            stack->Push(SEG_USER_DATA | RING3_MASK); // stackseg
            stack->Push((uint32_t)stack_virt);       // stack ptr
            stack->Push(0x202);                      // flags
            stack->Push(SEG_USER_CODE | RING3_MASK); // code seg
            stack->Push((uint32_t)entryPoint);       // ret ptr

            registers.ds = SEG_USER_DATA | RING3_MASK;
            registers.esp = (uint32_t)stack_virt;
        }
        

        // make sure we restore to kernel page directory before continuing
        if (ring == Ring::Ring3)
            PageDirectory::kernelDir->Load();
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