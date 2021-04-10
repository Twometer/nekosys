#include <kernel/kdebug.h>
#include <kernel/namedpipe.h>


namespace Kernel
{
    DEFINE_SINGLETON(PipeManager);

    PipeManager::PipeManager()
    {
    }

    NamedPipe *PipeManager::FindOrCreatePipe(const nk::String &name)
    {
        for (size_t i = 0; i < pipes.Size(); i++)
        {
            auto pipe = pipes.At(i);
            if (pipe->name == name)
            {
                return pipe;
            }
        }

        idCounter++;
        auto pipe = new NamedPipe();
        pipe->name = name;
        pipe->pipeId = idCounter;
        pipe->ownerProcess = Process::Current()->GetId();
        pipe->broken = false;
        pipes.Add(pipe);
        return pipe;
    }

    NamedPipe *PipeManager::FindPipe(uint32_t pipeId)
    {
        for (size_t i = 0; i < pipes.Size(); i++)
        {
            auto pipe = pipes.At(i);
            if (pipe->pipeId == pipeId)
            {
                return pipe;
            }
        }
        return nullptr;
    }

}; // namespace Kernel