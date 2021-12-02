#ifndef MEMORYMAP_HPP_
#define MEMORYMAP_HPP_

#include <stddef.h>
#include <stdint.h>

#include "BootInfo.hpp"

struct MemoryMapEntry {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi;
};

class MemoryMap {
   private:
    MemoryMapEntry *m_entries;
    size_t m_length;

   public:
    MemoryMap(BootInfo *boot_info);

    MemoryMapEntry *operator[](size_t index);

    MemoryMapEntry *find_largest_chunk();

    size_t length();
};

#endif  // MEMORYMAP_HPP_