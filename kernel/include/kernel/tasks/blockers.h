#ifndef _BLOCKERS_H
#define _BLOCKERS_H

#include <kernel/tasks/blocker.h>
#include <kernel/tasks/process.h>
#include <kernel/namedpipe.h>
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

    class PipeBlocker : public IThreadBlocker
    {
    private:
        pid_t me;
        NamedPipe *pipe;

    public:
        PipeBlocker(pid_t me, NamedPipe *pipe);

        bool IsBlocked() override;
    };

    class ThreadJoinBlocker : public IThreadBlocker
    {
    private:
        Thread *thread;

    public:
        ThreadJoinBlocker(Thread *thread);

        bool IsBlocked() override;
    };

}; // namespace Kernel

#endif