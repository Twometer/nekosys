#include <kernel/tasks/process.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/tasks/processdir.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/video/videomanager.h>
#include <kernel/fs/vfs.h>

using namespace Memory;

namespace Kernel
{
    Process *Process::current = nullptr;
    pid_t Process::idCounter = 0;

    Process::Process(nk::String name, Memory::PageDirectory *pageDir, nk::Vector<Kernel::Thread *> *threads, nk::Vector<void *> *pages)
        : pid(++idCounter), name(name), pageDir(pageDir), threads(threads), pages(pages),
          openFiles(new nk::Vector<uint32_t>()), shbufs(new nk::Vector<SharedBuffer>())
    {
    }

    Process::~Process()
    {
        delete pageDir;
        delete threads;
        delete pages;
        delete openFiles;
        delete shbufs;
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

        for (size_t i = 0; i < openFiles->Size(); i++)
            FS::VirtualFileSystem::GetInstance()->Close(openFiles->At(i));

        for (size_t i = 0; i < threads->Size(); i++)
            threads->At(i)->SetState(Kernel::ThreadState::Dead);

        // TODO also free the memory pages here

        if (Video::VideoManager::GetInstance()->GetFramebufferController() == pid)
        {
            Video::VideoManager::GetInstance()->AcquireFramebuffer(0);
        }

        if (current == this)
            Scheduler::GetInstance()->Yield();
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