#include <kernel/tasks/processdir.h>

namespace Kernel
{

    DEFINE_SINGLETON(ProcessDir)

    ProcessDir::ProcessDir()
    {
        processes = new nk::Vector<Process *>();
    }

    void ProcessDir::OnProcessStarted(Process *proc)
    {
        processes->Add(proc);
    }

    void ProcessDir::OnProcessDied(Process *proc)
    {
        for (size_t i = 0; i < processes->Size(); i++)
            if (processes->At(i)->GetId() == proc->GetId())
            {
                processes->Remove(i);
                break;
            }
    }

    Process *ProcessDir::GetProcess(pid_t pid)
    {
        for (size_t i = 0; i < processes->Size(); i++)
            if (processes->At(i)->GetId() == pid)
                return processes->At(i);
        return nullptr;
    }

    Memory::SharedBuffer *ProcessDir::FindShBuf(uint32_t bufid)
    {
        for (size_t i = 0; i < processes->Size(); i++)
        {
            auto process = processes->At(i);
            for (size_t j = 0; j < process->GetShbufs()->Size(); j++)
            {
                auto &buf = process->GetShbufs()->At(j);
                if (buf.GetBufId() == bufid)
                {
                    return &buf;
                }
            }
        }
        return nullptr;
    }

} // namespace Kernel