#ifndef BOOTINFO_HPP_
#define BOOTINFO_HPP_

#include <stdint.h>

#define BOOT_INFO_LOCATION     0x0500
#define MEM_MAP_RESULT_OK      0x00
#define MEM_MAP_RESULT_SIG     0x01
#define MEM_MAP_RESULT_BIOS    0x02
#define MEM_MAP_RESULT_UNKNOWN 0xFF

struct BootInfo {
    uint32_t mem_map_result;
    uint32_t mem_map_len;
    void* mem_map_ptr;
};

#endif  // BOOTINFO_HPP_