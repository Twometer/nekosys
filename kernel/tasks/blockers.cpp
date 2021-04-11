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

    PipeBlocker::PipeBlocker(pid_t me, NamedPipe *pipe)
        : me(me), pipe(pipe)
    {
    }

    bool PipeBlocker::IsBlocked()
    {
        if (pipe->broken)
            return false;

        for (size_t i = 0; i < pipe->packets->Size(); i++)
        {
            if (pipe->packets->At(i)->dstProcess == me)
                return false;
        }
        return true;
    }

    ThreadJoinBlocker::ThreadJoinBlocker(Thread *thread)
        : thread(thread)
    {
    }

    bool ThreadJoinBlocker::IsBlocked()
    {
        return thread->GetState() != ThreadState::Dead;
    }

}; // namespace Kernel