#ifndef _REGISTERS_H
#define _REGISTERS_H

#include <stdint.h>

struct RegisterStates
{
    uint32_t eax = 0x00;
    uint32_t ebx = 0x00;
    uint32_t ecx = 0x00;
    uint32_t edx = 0x00;
    uint32_t esp = 0x00;
    uint32_t esi = 0x00;
    uint32_t edi = 0x00;
    uint32_t ebp = 0x00;
    uint32_t flags = 0x00;
    uint32_t ds = 0x00;

    void CopyTo(RegisterStates *other)
    {
        other->eax = eax;
        other->ebx = ebx;
        other->ecx = ecx;
        other->edx = edx;
        other->esp = esp;
        other->esi = esi;
        other->edi = edi;
        other->ebp = ebp;
        other->flags = flags;
        other->ds = ds;
    }

} __attribute__((packed));

#endif