#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>

typedef uint8_t *pageframe_t;
typedef uint8_t *vaddress_t;
typedef uint8_t *paddress_t;

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN_DOWN(addr) (uint8_t *)((uint32_t)(addr) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (uint8_t *)(((uint32_t)(addr) + PAGE_SIZE) & ~(PAGE_SIZE - 1))
#define IS_PAGE_ALIGNED(addr) (PAGE_ALIGN_DOWN(addr) == (uint8_t *)addr)

#define PAGE_BIT_ALLOW_USER (1 << 2)
#define PAGE_BIT_READ_WRITE (1 << 1)
#define PAGE_BIT_PRESENT (1 << 0)

namespace Kernel
{
    class PageManager
    {
    private:
        uint8_t *memory_base;
        uint32_t memory_size;

        // todo we should probably support more than those two
        uint32_t *page_directory;
        uint32_t *page_table;

        uint8_t *frame_map;
        uint32_t num_pages;

        uint8_t *pageframes_base;

        uint32_t last_page_idx = 0;

    public:
        void Initialize(void *memory_base, uint32_t memory_size);

        void EnablePaging();

        pageframe_t AllocPageframe();

        void FreePageframe(pageframe_t frame);

        // void MapPage(pageframe_t page, vaddress_t addr);
    };

}; // namespace Kernel

#endif