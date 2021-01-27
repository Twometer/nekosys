#include <kernel/tasks/blockers.h>
#include <kernel/timemanager.h>
#include <kernel/device/devicemanager.h>

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

    KeyboardBlocker::KeyboardBlocker(char delim)
        : delim(delim)
    {
    }

    bool KeyboardBlocker::IsBlocked()
    {
        return !Device::DeviceManager::keyboard->GetBuffer().Contains(delim);
    }

}; // namespace Kernel