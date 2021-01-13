#ifndef _REGISTERS_H
#define _REGISTERS_H

#include <stdint.h>

struct RegisterStates
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esp;
    uint32_t esi;
    uint32_t edi;
} __attribute__((packed));

#endif