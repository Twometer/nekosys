#ifndef _PAGE_DIRECTORY_H
#define _PAGE_DIRECTORY_H

#include "memdefs.h"
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
        static PageDirectory *current;
        static PageDirectory *kernelDir;

    public:
        PageDirectory();

        PageDirectory(const PageDirectory &other);

        void MapSelf();

        void Load();

        bool MapPage(paddress_t physicalAddr, vaddress_t virtualAddr, uint16_t permissions);

        void *GetPhysicalAddress() { return phys_directory_ptr; }

        bool IsCurrent() { return current == this; };

        static PageDirectory *Current() { return current; }

        static PageDirectory *GetKernelDir() { return kernelDir; }

        static void SetKernelDir(PageDirectory *dir) { kernelDir = dir; }

    private:
        uint32_t *NewPage();

        uint32_t *GetPageTable(size_t idx);

        static inline void FlushPage(uint32_t addr);
    };

}; // namespace Memory

#endif