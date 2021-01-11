#ifndef _MEMORY_MAP_H
#define _MEMORY_MAP_H

#include <stdint.h>

namespace Memory
{

    struct MemoryMapHeader
    {
        uint32_t length;
        uint32_t result;
    };

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
        MemoryMapHeader *header = nullptr;
        MemoryMapEntry *entries = nullptr;

    public:
        void Parse(void *basePtr);

        MemoryMapEntry *GetEntry(int idx);

        uint32_t GetLength();

        void *GetLargestChunk();
    };

}; // namespace Memory

#endif