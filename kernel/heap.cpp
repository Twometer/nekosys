#include <device/cpu.h>
#include <string.h>
extern "C"
{
#include <kernel/heap.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

    /*
 * Simple heap implementation for getting the kernel started.
 * Should probably be replaced with something more efficient in
 * the future.
 * For now, we create a 1MB heap, and divide that into 128 byte
 * chunks, which we mark as used or unused in a bitmap.
 */

#define HEAP_OFFSET 0x1ffff // Where the heap starts
#define HEAP_SIZE 1000000   // 1MB heap size
#define HEAP_SEG_SIZE 128   // 128 byte allocation units

#define ALLOCATION_MAP_ENTRIES HEAP_SIZE / HEAP_SEG_SIZE
#define ALLOCATION_MAP_SIZE HEAP_SIZE / HEAP_SEG_SIZE / 8

    uint8_t *allocation_map;
    uint8_t *heap_begin;

    typedef struct
    {
        size_t idx;
        size_t size;
    } heap_entry;

    void heap_init()
    {
        allocation_map = (uint8_t *)(HEAP_OFFSET);
        heap_begin = allocation_map + ALLOCATION_MAP_SIZE;
        memset(allocation_map, 0x00, ALLOCATION_MAP_SIZE); // All zero: Nothing allocated
    }

    bool is_free(size_t alloc_unit)
    {
        size_t a = alloc_unit / 8;
        size_t b = alloc_unit % 8;
        return (allocation_map[a] & (0x1 << b)) == 0;
    }

    void set_free(size_t alloc_unit, bool free)
    {
        size_t a = alloc_unit / 8;
        size_t b = alloc_unit % 8;
        if (!free)
            allocation_map[a] = allocation_map[a] | (0x1 << b); // Set bit
        else
            allocation_map[a] = allocation_map[a] & ~(0x1 << b); // Clear bit
    }

    int find_free_allocation_units(size_t num_alloc_units)
    {
        for (size_t i = 0; i < ALLOCATION_MAP_ENTRIES; i++)
        {
            if (is_free(i))
            {

                bool found = true;
                size_t num = 0;
                for (size_t j = i; j < ALLOCATION_MAP_ENTRIES; j++)
                {
                    if (!is_free(j))
                    {
                        found = false;
                        break;
                    }
                    num++;
                    if (num == num_alloc_units)
                        break;
                }

                if (found)
                    return i;
            }
        }
        return -1;
    }

    int calc_num_alloc_units(int bytes)
    {
        return (bytes + HEAP_SEG_SIZE - 1) / HEAP_SEG_SIZE;
    }

    void *malloc(size_t size)
    {
        size_t header_size = sizeof(heap_entry);
        size_t total_size = size + header_size;
        size_t num_alloc_units = calc_num_alloc_units(total_size);

        int alloc_result = find_free_allocation_units(num_alloc_units);
        if (alloc_result < 0)
        {
            printf("kernel_heap: Out of memory! No more space for %d bytes.\n", size);
            Device::CPU::Halt();
            return 0;
        }

        size_t alloc_unit = (size_t)(alloc_result);
        for (size_t i = 0; i < num_alloc_units; i++)
            set_free(alloc_unit + i, false);

        uint8_t *slot = heap_begin + alloc_unit * HEAP_SEG_SIZE;
        uint8_t *data = slot + sizeof(heap_entry);

        heap_entry entry;
        entry.idx = alloc_unit;
        entry.size = num_alloc_units;
        memcpy(slot, &entry, sizeof(entry));

        return data;
    }

    void free(void *ptr)
    {
        uint8_t *byte_ptr = (uint8_t *)ptr;
        heap_entry *description = (heap_entry *)(byte_ptr - sizeof(heap_entry));

        for (size_t i = 0; i < description->size; i++)
            set_free(description->idx + i, true);
    }
}