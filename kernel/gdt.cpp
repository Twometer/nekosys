#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kernel/gdt.h>
#include <kernel/panic.h>
#include <kernel/memory/memdefs.h>

using namespace Kernel;

GDT *GDT::current = nullptr;

extern "C"
{
    extern void setGdt(void *, uint32_t);
    extern void setTss(uint32_t selector);
}

DEFINE_SINGLETON(GDT)

GDT::GDT()
{
}

void GDT::Create(size_t numEntries)
{
    this->numEntries = numEntries;
    this->entries = new GDTEntry[numEntries];
    memset(this->entries, 0x00, numEntries * sizeof(GDTEntry));
}

void GDT::Set(uint32_t selector, uint32_t base, uint32_t limit, GDTEntryType type, Ring ring)
{
    if (limit > 0xFFFF && (limit & 0xFFF) != 0xFFF)
    {
        Kernel::Panic("gdt_loader", "Limit %x cannot be encoded.", limit);
    }

    if ((selector % 0x08) != 0)
    {
        Kernel::Panic("gdt_loader", "Bad selector %x\n", selector);
    }

    // Convert to page addresses
    int granularity = 0;
    if (limit > 0xFFFF)
    {
        limit >>= 12;
        granularity = 1;
    }

    GDTEntry &entry = entries[selector / 0x08];
    entry.limit_low = limit & 0xFFFF; // low 16 bits
    entry.base_low = base & 0xFFFFFF; // low 24 bits
    entry.accessed = 0;
    entry.read_write = 1;
    entry.conforming = 0;
    entry.code = (uint32_t)type;
    entry.is_not_tss = 1;
    entry.dpl = (uint32_t)ring;
    entry.present = 1;
    entry.limit_high = (limit >> 16) & 0xF; // high 4 bits
    entry.available = 1;
    entry.always_0 = 0;
    entry.big = 1;
    entry.granularity = granularity;
    entry.base_high = (base >> 24) & 0xFF; // high 8 bits

    if (type == GDTEntryType::Null)
    {
        entry.accessed = 0;
        entry.read_write = 0;
        entry.conforming = 0;
        entry.code = 0;
        entry.is_not_tss = 0;
        entry.dpl = 0;
        entry.present = 0;
    }
}

void GDT::CreateTss(uint32_t selector)
{
    uint32_t base = (uint32_t)&tssEntry;
    uint32_t limit = sizeof(tssEntry);

    GDTEntry &entry = entries[selector / 0x08];
    entry.limit_low = limit & 0xFFFF;
    entry.base_low = base & 0xFFFFFF;           //isolate bottom 24 bits
    entry.accessed = 1;                         //This indicates it's a TSS and not a LDT. This is a changed meaning
    entry.read_write = 0;                       //This indicates if the TSS is busy or not. 0 for not busy
    entry.conforming = 0;                       //always 0 for TSS
    entry.code = 1;                             //For TSS this is 1 for 32bit usage, or 0 for 16bit.
    entry.is_not_tss = 0;                       //indicate it is a TSS
    entry.dpl = 3;                              //same meaning
    entry.present = 1;                          //same meaning
    entry.limit_high = (limit & 0xF0000) >> 16; //isolate top nibble
    entry.available = 0;
    entry.always_0 = 0;                          //same thing
    entry.big = 0;                               //should leave zero according to manuals. No effect
    entry.granularity = 0;                       //so that our computed GDT limit is in bytes, not pages
    entry.base_high = (base & 0xFF000000) >> 24; //isolate top byte.

    memset(&tssEntry, 0, sizeof(tssEntry));
    tssEntry.ss0 = SEG_KRNL_DATA;
    tssEntry.esp0 = (uint32_t)(new uint8_t[4096] + 4096); // todo wtf should i put here

    tssSelector = selector;
}

void GDT::FlushGdt()
{
    setGdt(entries, numEntries * sizeof(GDTEntry));
    current = this;
}

void GDT::FlushTss()
{
    setTss(tssSelector);
}