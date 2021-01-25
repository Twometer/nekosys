#ifndef _ELF_LOADER_H
#define _ELF_LOADER_H

#include <kernel/tasks/process.h>
#include <elf/elf.h>

namespace Kernel
{

    class ElfLoader
    {
    public:
        static Process *CreateProcess(const ELF::Image &image, int argc, char **argv);

    private:
        static void *MapNewZeroedPages(Memory::PageDirectory *dir, vaddress_t vaddr, size_t sizeInBytes);
    };

} // namespace Kernel

#endif