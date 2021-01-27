#include <stdint.h>
#include <kernel/memory/memorymap.h>
#include <kernel/panic.h>
#include <stdio.h>

using namespace Kernel;

namespace Memory
{

    void MemoryMap::Parse(KernelHandover *handover)
    {
        entries = (MemoryMapEntry *)handover->mmapPtr;
        length = handover->mmapLength;

        if (handover->mmapState == 1 && length == 0)
        {
            Kernel::Panic("memory_map", "Empty memory map");
        }
        else if (handover->mmapState == 2)
        {
            Kernel::Panic("memory_map", "Bad BIOS signature");
        }
        else if (handover->mmapState == 3)
        {
            Kernel::Panic("memory_map", "BIOS not available");
        }
    }

    MemoryMapEntry *MemoryMap::GetEntry(int idx)
    {
        return &entries[idx];
    }

    uint32_t MemoryMap::GetLength()
    {
        return length;
    }

    MemoryMapEntry *MemoryMap::GetLargestChunk()
    {
        MemoryMapEntry *largestEntry = GetEntry(0);
        for (uint32_t i = 0; i < GetLength(); i++)
        {
            auto *entry = GetEntry(i);
            if (entry->type == 0x01 && entry->lengthLow > largestEntry->lengthLow)
                largestEntry = entry;
        }
        return largestEntry;
    }

}; // namespace Memory