#include <stdint.h>
#include <memory/MemoryMap.h>
#include <kernel/panic.h>

namespace Memory
{

    void MemoryMap::Parse(uint8_t *basePtr)
    {
        header = (MemoryMapHeader *)basePtr;
        entries = (MemoryMapEntry *)(basePtr + sizeof(MemoryMapHeader));

        if (header->result == 1 && header->length == 0)
        {
            Kernel::Panic("memory_map", "Empty memory map");
        }
        else if (header->result == 2)
        {
            Kernel::Panic("memory_map", "Bad BIOS signature");
        }
        else if (header->result == 3)
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
        return header->length;
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