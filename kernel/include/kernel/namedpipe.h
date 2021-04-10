#ifndef _NAMED_PIPE_H
#define _NAMED_PIPE_H

#include <stddef.h>
#include <stdint.h>
#include <nk/vector.h>
#include <nk/singleton.h>
#include <kernel/tasks/process.h>

namespace Kernel
{

    struct PipePacket
    {
        pid_t srcProcess;
        pid_t dstProcess;
        size_t size;
        uint8_t *data;
    };

    struct NamedPipe
    {
        nk::String name;
        uint32_t pipeId;
        pid_t ownerProcess;
        bool broken = false;
        nk::Vector<PipePacket *> *packets;

        NamedPipe() {
            packets = new nk::Vector<PipePacket*>();
        }

        ~NamedPipe() {
            delete packets;
        }
    };

    class PipeManager
    {
        DECLARE_SINGLETON(PipeManager);

    private:
        nk::Vector<NamedPipe *> pipes{};

        uint32_t idCounter = 0;

    public:
        NamedPipe *FindOrCreatePipe(const nk::String &name);

        NamedPipe *FindPipe(uint32_t pipeId);
    };

}; // namespace Kernel

#endif