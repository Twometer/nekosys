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

} // namespace Kernel