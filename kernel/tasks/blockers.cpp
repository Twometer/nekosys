#include <kernel/tasks/blockers.h>
#include <kernel/timemanager.h>

namespace Kernel
{

    SleepThreadBlocker::SleepThreadBlocker(uint32_t timeout)
    {
        this->unblockTime = TimeManager::GetInstance()->GetUptime() + timeout;
    }

    bool SleepThreadBlocker::IsBlocked()
    {
        return TimeManager::GetInstance()->GetUptime() < unblockTime;
    }

    ProcessWaitBlocker::ProcessWaitBlocker(Process *proc)
    {
        this->proc = proc;
    }

    bool ProcessWaitBlocker::IsBlocked()
    {
        return proc->IsRunning();
    }

}; // namespace Kernel