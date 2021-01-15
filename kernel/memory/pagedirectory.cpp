#include <stdio.h>
#include <memory/pagemanager.h>
#include <memory/pagedirectory.h>

namespace Memory
{

    PageDirectory::PageDirectory()
    {
        directory_ptr = NewPage();

        // Mark all entries as not present
        for (int i = 0; i < 1024; i++)
            directory_ptr[i] = 0x00;
    }

    uint32_t *PageDirectory::NewPage()
    {
        return (uint32_t *)PageManager::GetInstance()->AllocPageframe();
    }

    bool PageDirectory::MapPage(vaddress_t physicalAddr, vaddress_t virtualAddr, uint16_t permissions)
    {
        if (!IS_PAGE_ALIGNED(physicalAddr) || !IS_PAGE_ALIGNED(virtualAddr))
            return false;

        uint32_t absolutePage = (uint32_t)physicalAddr / PAGE_SIZE;
        size_t directoryIdx = absolutePage / 1024;
        size_t pageIdx = absolutePage % 1024;

        permissions |= PAGE_BIT_PRESENT;
        uint32_t tableEntry = (uint32_t)virtualAddr | (permissions & 0xFFF);

        GetPageTable(directoryIdx)[pageIdx] = tableEntry;
        return true;
    }

    void PageDirectory::Load()
    {
        PageManager::GetInstance()->LoadPageDirectory(directory_ptr);
    }

    uint32_t *PageDirectory::GetPageTable(size_t idx)
    {
        auto table = directory_ptr[idx] & 0xFFFFF000;

        if (table == 0x00) // Allocate page table if it does not exist
        {
            table = (uint32_t)NewPage();
            directory_ptr[idx] = table | PAGE_BIT_PRESENT | PAGE_BIT_READ_WRITE;
        }

        return (uint32_t *)table;
    }

}; // namespace Memory