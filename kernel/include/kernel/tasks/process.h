#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <nk/vector.h>
#include <nk/string.h>
#include <kernel/tasks/thread.h>
#include <sys/types.h>
#include <signal.h>

namespace Kernel
{
    class Thread;
    class Process
    {
    private:
        static Process *current;
        static pid_t idCounter;

        pid_t pid;

        nk::String name;
        Memory::PageDirectory *pageDir;
        nk::Vector<Kernel::Thread *> *threads;
        nk::Vector<void *> *pages;

        nk::String cwd = "/";

        uint8_t *heapBase = 0;

        int exitCode;

    public:
        Process(nk::String name, Memory::PageDirectory *pageDir, nk::Vector<Kernel::Thread *> *threads, nk::Vector<void *> *pages);

        ~Process();

        void Start();

        void Crash(int signal);

        void Exit(int exitCode);

        void *MapNewPages(size_t num);

        pid_t GetId() { return pid; }

        bool IsRunning();

        int GetExitCode() { return exitCode; }
        void SetExitCode(int exitCode) { this->exitCode = exitCode; }

        void SetHeapBase(void *heapBase) { this->heapBase = (uint8_t *)heapBase; }
        uint8_t *GetHeapBase() { return heapBase; }

        nk::String &GetCwd() { return cwd; };
        void SetCwd(const nk::String &cwd) { this->cwd = cwd; };

        static void SetCurrent(Process *process) { current = process; };
        static Process *Current() { return current; }
    };

} // namespace Kernel

#endif