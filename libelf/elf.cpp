#include <stdio.h>
#include <elf/elf.h>

namespace ELF
{

    Image::Image(uint8_t *data, size_t data_size)
        : data(data), data_size(data_size), elf_header((Elf32_Ehdr *)data), is_valid(false)
    {
        if (!CheckMagic())
        {
            printf("elf: Bad signature\n");
            return;
        }

        if (elf_header->e_ident[EI_CLASS] != ELFCLASS32)
        {
            printf("elf: Can't load 64-bit binaries on 32-bit system\n");
            return;
        }

        if (elf_header->e_ident[EI_DATA] != ELFDATA2LSB)
        {
            printf("elf: Byte order not supported\n");
        }

        if (elf_header->e_machine != EM_386)
        {
            printf("elf: Bad machine type\n");
            return;
        }

        if (elf_header->e_version != EV_CURRENT)
        {
            printf("elf: Bad version number\n");
            return;
        }

        if (elf_header->e_type != ET_EXEC)
        {
            printf("elf: Not an executable\n");
            return;
        }

        if (elf_header->e_entry == 0x00)
        {
            printf("elf: No entry point\n");
            return;
        }

        if (elf_header->e_phentsize != sizeof(Elf32_Phdr))
        {
            printf("elf: Unknown header size\n");
            return;
        }

        program_header = (Elf32_Phdr *)(data + elf_header->e_phoff);
        is_valid = true;
    }

    bool Image::CheckMagic()
    {
        auto ident = elf_header->e_ident;
        return ident[EI_MAG0] == 0x7f && ident[EI_MAG1] == 'E' && ident[EI_MAG2] == 'L' && ident[EI_MAG3] == 'F';
    }

}; // namespace ELF