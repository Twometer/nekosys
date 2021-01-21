#include <kernel/panic.h>
#include <device/devicemanager.h>
#include <string.h>
#include <kernel/heap.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

extern "C"
{
    /*
 * Simple heap implementation for getting the kernel started.
 * Should probably be replaced with something more efficient in
 * the future.
 * For now, we create a 1MB heap, and divide that into 128 byte
 * chunks, which we mark as used or unused in a bitmap.
 */
#define HEAP_DEBUG 0

#define HEAP_SEG_SIZE 128 // 128 byte allocation units

    //#define ALLOCATION_MAP_ENTRIES HEAP_SIZE / HEAP_SEG_SIZE
    //#define ALLOCATION_MAP_SIZE HEAP_SIZE / HEAP_SEG_SIZE / 8

    uint8_t *allocation_map;
    uint8_t *heap_begin;

    size_t allocation_map_entries = 0;
    size_t allocation_map_size = 0;

    typedef struct
    {
        size_t idx;
        size_t size;
    } heap_entry;

    void heap_init(void *heap_base, size_t heap_size)
    {
        allocation_map = (uint8_t *)(heap_base);
        allocation_map_entries = heap_size / HEAP_SEG_SIZE;
        allocation_map_size = allocation_map_entries / 8;

        heap_begin = allocation_map + allocation_map_size;
        memset(allocation_map, 0x00, allocation_map_size); // All zero: Nothing allocated
    }

    bool is_free(size_t alloc_unit)
    {
        size_t a = alloc_unit / 8;
        size_t b = alloc_unit % 8;
        return (allocation_map[a] & (0x1 << b)) == 0;
    }

    size_t get_free_heap()
    {
        size_t freeHeap = 0;
        for (size_t i = 0; i < allocation_map_entries; i++)
            if (is_free(i))
                freeHeap += HEAP_SEG_SIZE;
        return freeHeap;
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
        for (size_t i = 0; i < allocation_map_entries; i++)
        {
            if (is_free(i))
            {

                bool found = false;
                size_t num = 0;
                for (size_t j = i; j < allocation_map_entries; j++)
                {
                    if (!is_free(j))
                        break;

                    num++;
                    if (num == num_alloc_units)
                    {
                        found = true;
                        break;
                    }
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

    void *kmalloc(size_t size)
    {
        size_t header_size = sizeof(heap_entry);
        size_t total_size = size + header_size;
        size_t num_alloc_units = calc_num_alloc_units(total_size);

#if HEAP_DEBUG
        printf("kernel_heap: Allocating %d bytes (%d -> %d)\n", size, total_size, num_alloc_units);
#endif

        int alloc_result = find_free_allocation_units(num_alloc_units);
        if (alloc_result < 0)
        {
            Kernel::Panic("kernel_heap", "No more space for %d bytes.\n", size);
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

#if HEAP_DEBUG
        printf("kernel_heap: Allocated at %d\n", alloc_unit);
#endif

        return data;
    }

    void kfree(void *ptr)
    {
        if (ptr == nullptr)
            return;

        uint8_t *byte_ptr = (uint8_t *)ptr;
        heap_entry *description = (heap_entry *)(byte_ptr - sizeof(heap_entry));

#if HEAP_DEBUG
        printf("kernel_heap: Freeing %d units from %d\n", description->size, description->idx);
#endif

        for (size_t i = 0; i < description->size; i++)
            set_free(description->idx + i, true);
    }
}