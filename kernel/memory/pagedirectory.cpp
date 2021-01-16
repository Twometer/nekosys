#include <stdio.h>
#include <kernel/panic.h>
#include <memory/pagemanager.h>
#include <memory/pagedirectory.h>

namespace Memory
{

    PageDirectory *PageDirectory::current = nullptr;
    PageDirectory *PageDirectory::kernelDir = nullptr;

    PageDirectory::PageDirectory()
    {
        // Don't allow creation of a new page directory
        // after paging is enabled, as that would break
        // just about everything :p
        if (PageManager::GetInstance()->IsPagingEnabled())
        {
            Kernel::Panic("page_dir", "Cannot create an empty page dir after paging is on.");
        }

        phys_directory_ptr = NewPage();
        virt_directory_ptr = phys_directory_ptr;

        // Mark all entries as not present
        for (int i = 0; i < 1024; i++)
            virt_directory_ptr[i] = 0x00;
    }

    PageDirectory::PageDirectory(const PageDirectory &other)
    {
        phys_directory_ptr = NewPage();
        virt_directory_ptr = other.virt_directory_ptr;

        // Map the new directory page at a temporary location so that we can write to it
        // Map page goes to the OLD directory, because we copied its virtual pointer
        // (and thats also the same for all, so well)
        MapPage((paddress_t)phys_directory_ptr, (vaddress_t)PAGE_DIR_TEMP_LOC, PAGE_BIT_READ_WRITE);

        // Now, we copy the old directory
        uint32_t *tempDirPtr = (uint32_t *)PAGE_DIR_TEMP_LOC;
        for (int i = 0; i < 1024; i++)
            tempDirPtr[i] = other.virt_directory_ptr[i];

        // IMPORTANT: Now we have to overwrite the last entry to map to the new dir IN THE NEW TABLE
        tempDirPtr[1023] = (uint32_t)phys_directory_ptr | PAGE_BIT_PRESENT;

        // We then have to load it so that modifications only go to the new dir (it writes to virt_ptr)
        Load();
    }

    uint32_t *PageDirectory::NewPage()
    {
        return (uint32_t *)PageManager::GetInstance()->AllocPageframe();
    }

    bool PageDirectory::MapPage(paddress_t physicalAddr, vaddress_t virtualAddr, uint16_t permissions)
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

        if (PageManager::GetInstance()->IsPagingEnabled())
            FlushPage((uint32_t)physicalAddr);
        return true;
    }

    void PageDirectory::MapSelf()
    {
        // The last entry should map to itself, so that all page tables as well as the
        // page directory are automatically mapped.

        virt_directory_ptr[1023] = (uint32_t)phys_directory_ptr | PAGE_BIT_PRESENT;
    }

    void PageDirectory::Load()
    {
        virt_directory_ptr = (uint32_t *)PAGEDIR_VIRTUAL_LOC;
        PageManager::GetInstance()->LoadPageDirectory(phys_directory_ptr);
        current = this;
    }

    uint32_t *PageDirectory::GetPageTable(size_t idx)
    {
        if (PageManager::GetInstance()->IsPagingEnabled())
        {
            if (virt_directory_ptr[idx] == 0x00)
            {
                auto table = (uint32_t)NewPage();
                virt_directory_ptr[idx] = table | PAGE_BIT_PRESENT | PAGE_BIT_READ_WRITE;
                FlushPage(table); // Flush TLB for that page, so that we for sure have that
                // Load();
            }

            return ((uint32_t *)PAGETABLES_VIRTUAL_LOC) + (0x400 * idx);
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

    void PageDirectory::FlushPage(uint32_t addr)
    {
        asm volatile("invlpg (%0)" ::"r"(addr)
                     : "memory");
    }

}; // namespace Memory