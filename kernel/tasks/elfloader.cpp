#include <kernel/tasks/elfloader.h>
#include <kernel/memory/memdefs.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/kdebug.h>
#include <kernel/fs/vfs.h>
#include <nk/path.h>
#include <stdio.h>
#include <string.h>

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

        nk::String name = nk::Path(path).GetFilename();
        ELF::Image image(buf, meta.size);
        auto proc = CreateProcess(name, image, argc, argv);

        delete[] buf;
        return proc;
    }

    Process *ElfLoader::CreateProcess(const nk::String &name, const ELF::Image &image, int argc, char **argv)
    {
        if (!image.IsValid())
            return nullptr;

        auto elfHeader = image.GetElfHeader();
        auto progHeaders = image.GetProgramHeaders();

        // If we've got no program headers, then
        // it's an object file which we can't run
        if (elfHeader->e_phnum == 0)
            return nullptr;

        // Copy the argv into kernel heap so that
        // we can access them from both
        auto argv_copy = new nk::String[argc];
        for (int i = 0; i < argc; i++)
            argv_copy[i] = argv[i];

        // Set up new page directory
        auto pages = new nk::Vector<void *>();
        auto oldPageDir = PageDirectory::Current();

        // FIXME: This could probably be made better, if the
        //        kernel directory had the global flag set so
        //        that we don't have to copy the stuff around.
        // We have to load the kernel directory here first,
        // because else we copy and overwrite stuff we don't
        // want.
        auto kernelDir = PageDirectory::GetKernelDir();
        kernelDir->Load();
        auto pagedir = new PageDirectory(*kernelDir);
        vaddress_t executableEndAddr = 0;

        // Load the ELF sections into the new process's memory
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

        // If the end address is STILL zero, then we
        // didn't load anything. We can't execute a
        // process like this.
        if (executableEndAddr == 0)
        {
            kdbg("elf_loader: error: No sections loaded\n");
            return nullptr;
        }

        // Copy argv into the new process's address space after
        // the end of the executable
        executableEndAddr = MapNewZeroedPages(pagedir, executableEndAddr, 4096); // We map a zeroed page, so that no kernel data is leaked
        char **argv_newproc = (char **)(executableEndAddr - 4096);               // This is the argv pointer for the NEW process
        vaddress_t argv_data = (vaddress_t)argv_newproc + sizeof(char *) * argc; // This is the begin of the array's data region

        for (int i = 0; i < argc; i++)
        {
            nk::String &value = argv_copy[i];
            argv_newproc[i] = (char *)argv_data;

            size_t len_with_nul = value.Length() + 1;
            memcpy(argv_data, value.CStr(), len_with_nul);
            argv_data += len_with_nul;
        }

        // Delete the kernel buffer
        delete[] argv_copy;

#if ELFLOADER_DEBUG
        kdbg("elf_loader: Making a stack at %x\n", executableEndAddr);
#endif
        pages->Add(MapNewZeroedPages(pagedir, (vaddress_t)executableEndAddr, 4096));
        auto stack = new Stack(executableEndAddr, 4096);
        stack->Push((uint32_t)argv_newproc); // Push the argv pointer on the stack
        stack->Push((uint32_t)argc);         // Push the number of args on the stack

        // Set up the main thread
#if ELFLOADER_DEBUG
        kdbg("elf_loader: Creating thread with ip=%x, sp=%x\n", elfHeader->e_entry, stack->GetStackPtr());
#endif
        auto thread = Thread::CreateUserThread((ThreadMain)elfHeader->e_entry, pagedir, stack);

        // Create and configure the process object
        auto threads = new nk::Vector<Thread *>();
        threads->Add(thread);

        auto process = new Process(name, pagedir, threads, pages);
        process->SetHeapBase(executableEndAddr + 4096);

        thread->SetProcess(process);

        // Return to previous memory space
        oldPageDir->Load();

        return process;
    }

    vaddress_t ElfLoader::MapNewZeroedPages(PageDirectory *dir, vaddress_t vaddr, size_t sizeInBytes)
    {
        if (!IS_PAGE_ALIGNED(vaddr))
        {
            kdbg("elf_loader: error: Can't make zeroed page at non-aligned address %x\n", vaddr);
            return nullptr;
        }

        size_t numPages = (size_t)PAGE_ALIGN_UP(sizeInBytes) / PAGE_SIZE;

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