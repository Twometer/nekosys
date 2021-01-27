#ifndef _BLOCKERS_H
#define _BLOCKERS_H

#include <kernel/tasks/blocker.h>
#include <kernel/tasks/process.h>
#include <stdint.h>

namespace Kernel
{

    class SleepThreadBlocker : public IThreadBlocker
    {
    private:
        uint32_t unblockTime;

    public:
        SleepThreadBlocker(uint32_t timeout);

        bool IsBlocked() override;
    };

    class ProcessWaitBlocker : public IThreadBlocker
    {
    private:
        Process *proc;

    public:
        ProcessWaitBlocker(Process *proc);

        bool IsBlocked() override;
    };

    class KeyboardBlocker : public IThreadBlocker
    {
    private:
        char delim;

    public:
        KeyboardBlocker(char delim);

        bool IsBlocked() override;
    };

}; // namespace Kernel

#endif