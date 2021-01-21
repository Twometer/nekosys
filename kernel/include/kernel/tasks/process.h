#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <nk/vector.h>
#include <kernel/tasks/thread.h>

typedef uint32_t pid_t;

class Process
{
    pid_t pid;
    nk::Vector<Kernel::Thread *> threads;

    void Kill();
};

#endif