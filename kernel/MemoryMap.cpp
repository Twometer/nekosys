#include "MemoryMap.hpp"

#include "Panic.hpp"

MemoryMap::MemoryMap(BootInfo* boot_info) {
    switch (boot_info->mem_map_result) {
    case MEM_MAP_RESULT_SIG:
        panic("memory_map", "Bad BIOS signature");
        return;
    case MEM_MAP_RESULT_BIOS:
        panic("memory_map", "Failed to query BIOS");
        return;
    case MEM_MAP_RESULT_UNKNOWN:
        panic("memory_map", "Unknown error");
        return;
    default:
        break;
    }

    m_entries = (MemoryMapEntry*)boot_info->mem_map_ptr;
    m_length = boot_info->mem_map_len;
}

MemoryMapEntry* MemoryMap::operator[](size_t index) {
    return &m_entries[index];
}

MemoryMapEntry* MemoryMap::find_largest_chunk() {
    auto largest = m_entries;
    for (size_t i = 0; i < m_length; i++) {
        auto entry = m_entries + i;
        if (entry->type == 0x01 && entry->length_low > largest->length_low) {
            largest = entry;
        }
    }
    return largest;
}

size_t MemoryMap::length() {
    return m_length;
}