#include <stdint.h>
#include <stdio.h>
#include <memory/pagemanager.h>
#include <kernel/panic.h>

extern "C"
{
    extern void enable_paging();
    extern void load_page_directory(uint32_t *page_directory);
}

namespace Kernel
{

#define FREE 0x00
#define USED 0x01

    void PageManager::Initialize(void *memory_base, uint32_t memory_size)
    {
        this->memory_base = (uint8_t *)memory_base;
        this->memory_size = memory_size;

        this->frame_map = this->memory_base;
        this->num_pages = memory_size / PAGE_SIZE;

        printf("Number of possible memory pages: %d\n", num_pages);

        this->pageframes_base = PAGE_ALIGN_UP(this->frame_map + this->num_pages);
        printf("Pageframe base: %x\n", this->pageframes_base);

        // Put page directory and table in page frames
        page_directory = (uint32_t *)AllocPageframe();
        page_table = (uint32_t *)AllocPageframe();

        // Set all page tables as not present
        for (int i = 0; i < 1024; i++)
            page_directory[i] = 0x00000002;

        // identity map the first 4 MB
        for (uint32_t i = 0; i < 1024; i++)
        {
            page_table[i] = (i * 0x1000) | PAGE_BIT_PRESENT | PAGE_BIT_READ_WRITE;
        }
       
        page_directory[0] = ((uint32_t)page_table) | PAGE_BIT_PRESENT | PAGE_BIT_READ_WRITE;
    }

    void PageManager::EnablePaging()
    {
        load_page_directory(page_directory);

        // todo set up directory before enabling
        enable_paging();
    }

    pageframe_t PageManager::AllocPageframe()
    {
        // todo make this more efficient
        for (uint32_t i = last_page_idx; i < num_pages; i++)
        {
            if (frame_map[i] == FREE)
            {
                frame_map[i] = USED;
                last_page_idx = i;
                return pageframes_base + i * PAGE_SIZE;
            }
        }
        Kernel::Panic("pageframe_alloc", "Out of memory!");
        return nullptr;
    }

    void PageManager::FreePageframe(pageframe_t pageframe)
    {
        auto idx = (uint32_t)(pageframe - pageframes_base) / PAGE_SIZE;
        if (idx < last_page_idx)
            last_page_idx = idx;
        frame_map[idx] = FREE;
    }

}; // namespace Kernel