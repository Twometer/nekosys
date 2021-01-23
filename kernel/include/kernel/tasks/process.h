#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <nk/vector.h>
#include <kernel/tasks/thread.h>

typedef uint32_t pid_t;

namespace Kernel
{
    class Thread;
    class Process
    {
    private:
        static Process *current;
        static pid_t idCounter;

        pid_t pid;

        Memory::PageDirectory *pageDir;
        nk::Vector<Kernel::Thread *> *threads;
        nk::Vector<void *> *pages;

        uint8_t *heapBase = 0;

    public:
        Process(Memory::PageDirectory *pageDir, nk::Vector<Kernel::Thread *> *threads, nk::Vector<void *> *pages);

        ~Process();

        void Start();

        void Crash();

        void Kill();

        void *MapNewPages(size_t num);

        pid_t GetId() { return pid; }

        void SetHeapBase(void *heapBase) { this->heapBase = (uint8_t *)heapBase; }
        uint8_t *GetHeapBase() { return heapBase; }

        static void SetCurrent(Process *process) { current = process; };
        static Process *Current() { return current; }
    };

} // namespace Kernel

#endif