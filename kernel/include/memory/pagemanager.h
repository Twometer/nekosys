#ifndef _PAGING_H
#define _PAGING_H

#include "memdefs.h"
#include <stdint.h>

namespace Memory
{
    class PageManager
    {
    private:
        static PageManager *instance;

        uint8_t *memory_base;
        uint32_t memory_size;

        uint8_t *frame_map;
        uint32_t num_pages;
        uint32_t last_page_idx = 0;

        uint8_t *pageframes_base;

    public:
        void Initialize(void *memory_base, uint32_t memory_size);

        void LoadPageDirectory(uint32_t *page_directory);

        void EnablePaging();

        pageframe_t AllocPageframe();

        void FreePageframe(pageframe_t frame);

        // void MapPage(pageframe_t page, vaddress_t addr);

        static PageManager *GetInstance() { return instance; }
    };

}; // namespace Memory

#endif