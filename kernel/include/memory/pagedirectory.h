#ifndef _PAGE_DIRECTORY_H
#define _PAGE_DIRECTORY_H

#include <stddef.h>
#include <stdint.h>

namespace Memory
{
    class PageDirectory
    {
    private:
        uint32_t *directory_ptr;

    public:
        PageDirectory();

        void Load();

        bool MapPage(paddress_t physicalAddr, vaddress_t virtualAddr, uint16_t permissions);

    private:
        uint32_t *NewPage();

        uint32_t *GetPageTable(size_t idx);
    };

}; // namespace Memory

#endif