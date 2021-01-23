#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>
#include <kernel/ring.h>
#include <nk/singleton.h>

namespace Kernel
{
    struct TSSEntry
    {
        uint32_t prev_tss; // The previous TSS - if we used hardware task switching this would form a linked list.
        uint32_t esp0;     // The stack pointer to load when we change to kernel mode.
        uint32_t ss0;      // The stack segment to load when we change to kernel mode.
        uint32_t esp1;     // everything below here is unusued now..
        uint32_t ss1;
        uint32_t esp2;
        uint32_t ss2;
        uint32_t cr3;
        uint32_t eip;
        uint32_t eflags;
        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
        uint32_t es;
        uint32_t cs;
        uint32_t ss;
        uint32_t ds;
        uint32_t fs;
        uint32_t gs;
        uint32_t ldt;
        uint16_t trap;
        uint16_t iomap_base;
    } __attribute__((packed));

    struct GDTEntry
    {
        unsigned int limit_low : 16;
        unsigned int base_low : 24;
        unsigned int accessed : 1;
        unsigned int read_write : 1; // read for code, write for data
        unsigned int conforming : 1;
        unsigned int code : 1;       // 1 for code, 0 for data
        unsigned int is_not_tss : 1; // 1 for everything but TSS and LDT
        unsigned int dpl : 2;        // privilege level
        unsigned int present : 1;
        unsigned int limit_high : 4;
        unsigned int available : 1;
        unsigned int always_0 : 1;
        unsigned int big : 1;         // 1 for 32-bit, 0 for 16-bit
        unsigned int granularity : 1; // 1 for 4k addressing, 0 for byte addressing
        unsigned int base_high : 8;

    } __attribute__((packed));

    enum class GDTEntryType
    {
        Data = 0x00,
        Code = 0x01,
        Null = 0x02
    };

    class GDT
    {
        DECLARE_SINGLETON(GDT);

    private:
        TSSEntry tssEntry;
        GDTEntry *entries;
        size_t numEntries;
        static GDT *current;

        uint32_t tssSelector = 0;

    public:
        void Create(size_t numEntries);

        void Set(uint32_t selector, uint32_t base, uint32_t limit, GDTEntryType type, Ring ring);

        void CreateTss(uint32_t selector);

        TSSEntry &GetTssEntry() { return tssEntry; }

        void FlushGdt();

        void FlushTss();

    private:
        void EncodeEntry(uint8_t *target, GDTEntry entry);
    };

}; // namespace Kernel

#endif