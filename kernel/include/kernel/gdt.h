#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define GDT_ENTRY_SIZE 8

namespace Kernel
{

    struct GDTEntry
    {
        uint32_t base;
        uint32_t limit;
        uint32_t type;
    };

    class GDT
    {
    private:
        uint8_t *data;
        uint32_t size;

    public:
        GDT(int entries);

        void Set(int selector, const GDTEntry &entry);

        void Load();

    private:
        void EncodeEntry(uint8_t *target, GDTEntry entry);
    };

}; // namespace Kernel

#endif