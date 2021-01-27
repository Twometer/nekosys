#ifndef _MEMORY_MAP_H
#define _MEMORY_MAP_H

#include <kernel/handover.h>
#include <stdint.h>

namespace Memory
{
    struct MemoryMapEntry
    {
        uint32_t baseLow;
        uint32_t baseHigh;
        uint32_t lengthLow;
        uint32_t lengthHigh;
        uint32_t type;
        uint32_t acpi;
    };

    class MemoryMap
    {
    private:
        MemoryMapEntry *entries = nullptr;

        int length;

    public:
        void Parse(Kernel::KernelHandover *handover);

        MemoryMapEntry *GetEntry(int idx);

        uint32_t GetLength();

        MemoryMapEntry *GetLargestChunk();
    };

}; // namespace Memory

#endif