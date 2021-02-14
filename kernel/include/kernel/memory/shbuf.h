#ifndef _SHBUF_H
#define _SHBUF_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/memory/memdefs.h>

namespace Memory
{

    class SharedBuffer
    {
    private:
        static uint32_t idCounter;

        uint32_t bufid;
        pageframe_t baseFrame;
        size_t numPages;

    public:
        SharedBuffer()
        {
        }

        SharedBuffer(pageframe_t base, size_t numPages);

        uint32_t GetBufId() const { return bufid; }

        pageframe_t GetBaseFrame() const { return baseFrame; }

        size_t GetNumPages() const { return numPages; }
    };

}; // namespace Memory

#endif