#include <stdio.h>
#include <memory/pagemanager.h>
#include <memory/pagedirectory.h>

namespace Memory
{

#define PAGEDIR_VIRTUAL_LOC 0xFFFFF000

    PageDirectory::PageDirectory()
    {
        phys_directory_ptr = NewPage();
        virt_directory_ptr = phys_directory_ptr;

        // Mark all entries as not present
        for (int i = 0; i < 1024; i++)
            virt_directory_ptr[i] = 0x00;
    }

    uint32_t *PageDirectory::NewPage()
    {
        return (uint32_t *)PageManager::GetInstance()->AllocPageframe();
    }

    bool PageDirectory::MapPage(vaddress_t physicalAddr, vaddress_t virtualAddr, uint16_t permissions)
    {
        if (!IS_PAGE_ALIGNED(physicalAddr) || !IS_PAGE_ALIGNED(virtualAddr))
            return false;

        uint32_t absolutePage = (uint32_t)virtualAddr / PAGE_SIZE;
        size_t directoryIdx = absolutePage / 1024;
        size_t pageIdx = absolutePage % 1024;

        permissions |= PAGE_BIT_PRESENT;
        uint32_t tableEntry = (uint32_t)physicalAddr | (permissions & 0xFFF);

#if PAGE_DBG
        printf("%x: %x[%x]=%x\n", absolutePage, directoryIdx, pageIdx, tableEntry);
        printf("  %x -> %x\n", physicalAddr, virtualAddr);
#endif

        GetPageTable(directoryIdx)[pageIdx] = tableEntry;
        return true;
    }

    // todo page tables also need to be mapped

    void PageDirectory::MapSelf()
    {
        //MapPage((paddress_t)phys_directory_ptr, (vaddress_t)PAGEDIR_VIRTUAL_LOC, PAGE_BIT_READ_WRITE);
        phys_directory_ptr[1023] = (uint32_t)phys_directory_ptr | PAGE_BIT_PRESENT;
        virt_directory_ptr = (uint32_t *)PAGEDIR_VIRTUAL_LOC;
    }

    void PageDirectory::Load()
    {
        PageManager::GetInstance()->LoadPageDirectory(phys_directory_ptr);
        paging_enabled = true;
    }

    uint32_t *PageDirectory::GetPageTable(size_t idx)
    {
        if (paging_enabled)
        {
            if (virt_directory_ptr[idx] == 0x00)
            {
                auto table = (uint32_t)NewPage();
                virt_directory_ptr[idx] = table | PAGE_BIT_PRESENT | PAGE_BIT_READ_WRITE;
                Load();
            }

            return ((uint32_t *)0xFFC00000) + (0x400 * idx);
        }
        else
        {
            auto table = virt_directory_ptr[idx] & 0xFFFFF000;
            if (table == 0x00) // Allocate page table if it does not exist
            {
                table = (uint32_t)NewPage();
                virt_directory_ptr[idx] = table | PAGE_BIT_PRESENT | PAGE_BIT_READ_WRITE;
            }
            return (uint32_t *)table;
        }
    }

}; // namespace Memory