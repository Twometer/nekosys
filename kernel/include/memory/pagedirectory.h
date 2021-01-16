#ifndef _PAGE_DIRECTORY_H
#define _PAGE_DIRECTORY_H

#include <stddef.h>
#include <stdint.h>

namespace Memory
{
    class PageDirectory
    {
    private:
        uint32_t *phys_directory_ptr;
        uint32_t *virt_directory_ptr;
        bool paging_enabled = false;

    public:
        PageDirectory();

        void MapSelf();

        void Load();

        bool MapPage(paddress_t physicalAddr, vaddress_t virtualAddr, uint16_t permissions);

    private:
        uint32_t *NewPage();

        uint32_t *GetPageTable(size_t idx);

        static inline void FlushPage(uint32_t addr);
    };

}; // namespace Memory

#endif