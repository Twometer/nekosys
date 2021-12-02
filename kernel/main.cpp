#include <stdint.h>

#include "BootInfo.hpp"
#include "MemoryMap.hpp"

static inline uint16_t make_vga_entry(unsigned char chr, uint8_t color) {
    return (uint16_t)chr | (uint16_t)color << 8;
}

extern "C" void kmain(void) {
    uint16_t *vga_buffer = (uint16_t *)0xB8000;
    vga_buffer[0] = make_vga_entry('H', 0x0b);
    vga_buffer[1] = make_vga_entry('e', 0x0b);
    vga_buffer[2] = make_vga_entry('l', 0x0b);
    vga_buffer[3] = make_vga_entry('l', 0x0b);
    vga_buffer[4] = make_vga_entry('o', 0x0b);

    BootInfo *info = (BootInfo *)BOOT_INFO_LOCATION;
    vga_buffer[7] = make_vga_entry('0' + info->mem_map_result, 0x0c);

    MemoryMap memory_map(info);
}