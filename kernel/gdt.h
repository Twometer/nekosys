#ifndef _GDT_H
#define _GDT_H

#include <stddef.h>
#include <stdint.h>

typedef struct GDT {
    size_t base;
    size_t limit;
    size_t type;
} GDT_entry;

void gdt_initialize();
void gdt_encode_entry(uint8_t *target, GDT_entry source);

#endif
