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

}; // namespace Kernel