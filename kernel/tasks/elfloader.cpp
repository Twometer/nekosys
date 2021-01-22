#include <kernel/tasks/elfloader.h>
#include <kernel/memory/memdefs.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <stdio.h>
#include <string.h>

#define ELFLOADER_DEBUG 0

using namespace Memory;

namespace Kernel
{

    Thread *ElfLoader::LoadElf(const ELF::Image &image)
    {
        if (!image.IsValid())
            return nullptr;

        auto elfHeader = image.GetElfHeader();
        auto progHeaders = image.GetProgramHeaders();

        if (elfHeader->e_phnum == 0)
            return nullptr;

        auto pagedir = new PageDirectory(*PageDirectory::GetKernelDir());
        void *executableEndAddr = 0;

        for (size_t i = 0; i < elfHeader->e_phnum; i++)
        {
            auto &header = progHeaders[i];
            if (header.p_type != PT_LOAD)
                continue;

            auto page = PAGE_ALIGN_DOWN(header.p_vaddr);
            executableEndAddr = MapNewZeroedPages(pagedir, page, header.p_filesz);

#if ELFLOADER_DEBUG
            printf("elf_loader: Loading %d bytes from %x to %x (page %x)\n", header.p_filesz, header.p_offset, header.p_vaddr, PAGE_ALIGN_DOWN(header.p_vaddr));
#endif
            memcpy((void *)header.p_vaddr, (void *)(image.GetData() + header.p_offset), header.p_filesz);
        }

        if (executableEndAddr == 0)
        {
            printf("elf_loader: error: No address loaded\n");
            return nullptr;
        }

#if ELFLOADER_DEBUG
        printf("elf_loader: Making a stack at %x\n", executableEndAddr);
#endif
        MapNewZeroedPages(pagedir, (vaddress_t)executableEndAddr, 4096);
        auto stack = new Stack(executableEndAddr, 4096);
        stack->Push(0x00);
        stack->Push(0x00);

        // Set up thread
#if ELFLOADER_DEBUG
        printf("elf_loader: Creating thread with ip=%x, sp=%x\n", elfHeader->e_entry, stack->GetStackPtr());
#endif
        auto thread = Thread::CreateUserThread((ThreadMain)elfHeader->e_entry, pagedir, stack);
        thread->SetFreeMemBase((uint32_t)(executableEndAddr + 4096));

        // Return to kernel
        PageDirectory::GetKernelDir()->Load();

        return thread;
    }

    void *ElfLoader::MapNewZeroedPages(PageDirectory *dir, vaddress_t vaddr, size_t sizeInBytes)
    {
        if (!IS_PAGE_ALIGNED(vaddr))
        {
            printf("elf_loader: error: Can't make zeroed page at non-aligned address %x\n", vaddr);
            return nullptr;
        }

        auto numPages = (int)PAGE_ALIGN_UP(sizeInBytes) / PAGE_SIZE;

#if ELFLOADER_DEBUG
        printf("elf_loader: Making %d new zeroed pages at virtual %x\n", numPages, vaddr);
#endif

        vaddress_t a = vaddr;
        for (size_t i = 0; i < numPages; i++)
        {
            auto pageframe = PageManager::GetInstance()->AllocPageframe();
            dir->MapPage(pageframe, a, PAGE_BIT_ALLOW_USER | PAGE_BIT_READ_WRITE);
            a += PAGE_SIZE;
        }

        
        memset(vaddr, 0, sizeInBytes);
        return a;
    }

} // namespace Kernel