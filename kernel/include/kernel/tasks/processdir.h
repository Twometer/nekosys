#ifndef _PROCESS_DIR_H
#define _PROCESS_DIR_H

#include <kernel/tasks/process.h>
#include <nk/vector.h>
#include <nk/singleton.h>

namespace Kernel
{

    class ProcessDir
    {
        DECLARE_SINGLETON(ProcessDir)

    private:
        nk::Vector<Process *> *processes;

    public:
        void OnProcessStarted(Process *proc);

        void OnProcessDied(Process *proc);

        Process *GetProcess(pid_t pid);
    };

}; // namespace Kernel

#endif