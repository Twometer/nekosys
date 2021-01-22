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
        auto stackAddr = 0;

        for (size_t i = 0; i < elfHeader->e_phnum; i++)
        {
            auto &header = progHeaders[i];
            if (header.p_type != PT_LOAD)
                continue;

            auto page = PAGE_ALIGN_DOWN(header.p_vaddr);
            MapNewZeroedPage(pagedir, page);

#if ELFLOADER_DEBUG
            printf("elf_loader: Loading %d bytes from %x to %x (page %x)\n", header.p_filesz, header.p_offset, header.p_vaddr, PAGE_ALIGN_DOWN(header.p_vaddr));
#endif
            memcpy((void *)header.p_vaddr, (void *)(image.GetData() + header.p_offset), header.p_filesz);

            if (stackAddr < header.p_vaddr)
                stackAddr = header.p_vaddr;
        }

        if (stackAddr == 0)
        {
            printf("elf_loader: error: No address loaded\n");
            return nullptr;
        }

        auto alignedStack = PAGE_ALIGN_UP(stackAddr);
#if ELFLOADER_DEBUG
        printf("elf_loader: Making a stack at %x\n", alignedStack);
#endif
        MapNewZeroedPage(pagedir, alignedStack);
        auto stack = new Stack(alignedStack, 4096);
        stack->Push(0x00);
        stack->Push(0x00);

        // Set up thread
#if ELFLOADER_DEBUG
        printf("elf_loader: Creating thread with ip=%x, sp=%x\n", elfHeader->e_entry, stack->GetStackPtr());
#endif
        auto thread = Thread::CreateUserThread((ThreadMain)elfHeader->e_entry, pagedir, stack);
        thread->SetFreeMemBase((uint32_t)(alignedStack + 4096));
        
        // Return to kernel
        PageDirectory::GetKernelDir()->Load();

        return thread;
    }

    void ElfLoader::MapNewZeroedPage(PageDirectory *dir, vaddress_t vaddr)
    {
        if (!IS_PAGE_ALIGNED(vaddr))
        {
            printf("elf_loader: error: Can't make zeroed page at non-aligned address %x\n", vaddr);
            return;
        }

#if ELFLOADER_DEBUG
        printf("elf_loader: Making new zeroed page at virtual %x\n", vaddr);
#endif

        auto pageframe = PageManager::GetInstance()->AllocPageframe();
        dir->MapPage(pageframe, vaddr, PAGE_BIT_ALLOW_USER | PAGE_BIT_READ_WRITE);
        memset(vaddr, 0, PAGE_SIZE);
    }

} // namespace Kernel