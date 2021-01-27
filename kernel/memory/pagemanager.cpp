#include <stdint.h>
#include <stdio.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/panic.h>
#include <kernel/kdebug.h>

extern "C"
{
    extern void enable_paging();
    extern void load_page_directory(uint32_t *page_directory);
}

namespace Memory
{
    PageManager *PageManager::instance = nullptr;

    void PageManager::Initialize(void *memory_base, uint32_t memory_size)
    {
        kdbg("Setting up paging\n");

        if (instance != nullptr)
            Kernel::Panic("page_manager", "Page manager initialized twice");
        instance = this;

        this->memory_base = (uint8_t *)memory_base;
        this->memory_size = memory_size;

        this->frame_map = this->memory_base;
        this->num_pages = memory_size / PAGE_SIZE;

        kdbg("  Number of possible memory pages: %d\n", num_pages);

        this->pageframes_base = PAGE_ALIGN_UP(this->frame_map + this->num_pages);
        kdbg("  Pageframe base: %x\n", this->pageframes_base);
    }

    void PageManager::LoadPageDirectory(uint32_t *page_directory)
    {
        load_page_directory(page_directory);
    }

    void PageManager::EnablePaging()
    {
        paging_enabled = true;
        enable_paging();
    }

    pageframe_t PageManager::AllocPageframe()
    {
        // FIXME: This can probably be more efficient
        for (uint32_t i = last_page_idx; i < num_pages; i++)
        {
            if (frame_map[i] == PAGE_FREE)
            {
                frame_map[i] = PAGE_USED;
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
        if (idx > num_pages)
            return;

        if (idx < last_page_idx)
            last_page_idx = idx;
        frame_map[idx] = PAGE_FREE;
    }

}; // namespace Memory