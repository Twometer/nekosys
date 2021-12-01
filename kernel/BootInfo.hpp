#ifndef BOOTINFO_HPP_
#define BOOTINFO_HPP_

#define BOOT_INFO_LOCATION 0x0500

struct BootInfo {
    uint32_t mem_map_result;
    uint32_t mem_map_len;
    void* mem_map_ptr;
};

#endif  // BOOTINFO_HPP_