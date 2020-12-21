#include <stdlib.h>
#include <stdio.h>
#include <kernel/gdt.h>
#include <kernel/panic.h>

using namespace Kernel;

extern "C" {
    extern void setGdt(void*, uint32_t);
}

GDT::GDT(int entries)
{
    this->size = entries * GDT_ENTRY_SIZE;
    this->data = new uint8_t[size];
}

void GDT::Set(int selector, const GDTEntry &entry)
{
    uint8_t *ptr = data + (selector * GDT_ENTRY_SIZE);
    EncodeEntry(ptr, entry);
}

void GDT::Load()
{
    setGdt(data, size * GDT_ENTRY_SIZE);
    printf("GDT loaded %s\n", data);
}

void GDT::EncodeEntry(uint8_t *target, GDTEntry source)
{
    // Check the limit to make sure that it can be encoded
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF))
    {
        Kernel::Panic("Invalid GDT Entry!");
    }
    if (source.limit > 65536)
    {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    }
    else
    {
        target[6] = 0x40;
    }

    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;

    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    // And... Type
    target[5] = source.type;
}