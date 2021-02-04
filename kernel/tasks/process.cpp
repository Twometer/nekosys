#include <kernel/tasks/process.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/tasks/processdir.h>

using namespace Memory;

namespace Kernel
{
    Process *Process::current = nullptr;
    pid_t Process::idCounter = 0;

    Process::Process(Memory::PageDirectory *pageDir, nk::Vector<Kernel::Thread *> *threads, nk::Vector<void *> *pages)
        : pid(++idCounter), pageDir(pageDir), threads(threads), pages(pages)
    {
    }

    Process::~Process()
    {
    }

    void Process::Start()
    {
        ProcessDir::GetInstance()->OnProcessStarted(this);
        threads->At(0)->Start();
    }

    void Process::Crash(int signal)
    {
        Exit(RCSIGBASE + signal);
    }

    void Process::Exit(int exitCode)
    {
        ProcessDir::GetInstance()->OnProcessDied(this);

        this->exitCode = exitCode;
        kdbg("User process %d died with exit code %d\n", pid, exitCode);

        for (size_t i = 0; i < threads->Size(); i++)
            threads->At(i)->Kill();
    }

    bool Process::IsRunning()
    {
        for (size_t i = 0; i < threads->Size(); i++)
            if (threads->At(i)->GetState() != ThreadState::Dead)
                return true;
        return false;
    }

    void *Process::MapNewPages(size_t num)
    {
        auto base = heapBase;
        for (size_t i = 0; i < num; i++)
        {
            auto pageframe = PageManager::GetInstance()->AllocPageframe();
            pages->Add(pageframe);
            pageDir->MapPage(pageframe, (vaddress_t)heapBase, PAGE_BIT_READ_WRITE | PAGE_BIT_ALLOW_USER);
            heapBase += PAGE_SIZE;
        }
        return (void *)base;
    }

} // namespace Kernel