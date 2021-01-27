#include <kernel/tasks/elfloader.h>
#include <kernel/memory/memdefs.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/kdebug.h>
#include <kernel/fs/vfs.h>
#include <stdio.h>
#include <string.h>

#define ELFLOADER_DEBUG 0

using namespace Memory;

namespace Kernel
{

    Process *ElfLoader::CreateProcess(const char *path, int argc, char **argv)
    {
        auto vfs = FS::VirtualFileSystem::GetInstance();
        auto meta = vfs->GetFileMeta(path);
        if (!meta.IsValid())
            return nullptr;

        auto buf = new uint8_t[meta.size];
        
        auto fd = vfs->Open(path);
        vfs->Read(fd, 0, meta.size, buf);
        vfs->Close(fd);

        ELF::Image image(buf, meta.size);
        auto proc = CreateProcess(image, argc, argv);

        delete[] buf;
        return proc;
    }

    Process *ElfLoader::CreateProcess(const ELF::Image &image, int argc, char **argv)
    {
        if (!image.IsValid())
            return nullptr;

        auto elfHeader = image.GetElfHeader();
        auto progHeaders = image.GetProgramHeaders();

        if (elfHeader->e_phnum == 0)
            return nullptr;

        auto pages = new nk::Vector<void *>();
        auto oldPageDir = PageDirectory::Current();

        auto kernelDir = PageDirectory::GetKernelDir();

        kernelDir->Load();
        auto pagedir = new PageDirectory(*kernelDir);
        void *executableEndAddr = 0;

        for (size_t i = 0; i < elfHeader->e_phnum; i++)
        {
            auto &header = progHeaders[i];
            if (header.p_type != PT_LOAD)
                continue;

            auto page = PAGE_ALIGN_DOWN(header.p_vaddr);
            pages->Add(executableEndAddr = MapNewZeroedPages(pagedir, page, header.p_filesz));

#if ELFLOADER_DEBUG
            kdbg("elf_loader: Loading %d bytes from %x to %x (page %x)\n", header.p_filesz, header.p_offset, header.p_vaddr, PAGE_ALIGN_DOWN(header.p_vaddr));
#endif
            memcpy((void *)header.p_vaddr, (void *)(image.GetData() + header.p_offset), header.p_filesz);
        }

        if (executableEndAddr == 0)
        {
            printf("elf_loader: error: No address loaded\n");
            return nullptr;
        }

#if ELFLOADER_DEBUG
        kdbg("elf_loader: Making a stack at %x\n", executableEndAddr);
#endif
        pages->Add(MapNewZeroedPages(pagedir, (vaddress_t)executableEndAddr, 4096));
        auto stack = new Stack(executableEndAddr, 4096);
        stack->Push((uint32_t)argc);
        stack->Push((uint32_t)argv);

        // Set up thread
#if ELFLOADER_DEBUG
        kdbg("elf_loader: Creating thread with ip=%x, sp=%x\n", elfHeader->e_entry, stack->GetStackPtr());
#endif
        auto thread = Thread::CreateUserThread((ThreadMain)elfHeader->e_entry, pagedir, stack);

        auto threads = new nk::Vector<Thread *>();
        threads->Add(thread);

        auto process = new Process(pagedir, threads, pages);
        process->SetHeapBase(executableEndAddr + 4096);

        thread->SetProcess(process);

        // Return to previous memory space
        oldPageDir->Load();

        return process;
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
        kdbg("elf_loader: Making %d new zeroed pages at virtual %x\n", numPages, vaddr);
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