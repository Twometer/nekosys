#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>

typedef struct IDT_entry {
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
} IDT_entry;

void interrupts_initialize();
void interrupts_disable();
void interrupts_enable();

static inline void interrupts_lidt(void *base, uint16_t size) {
  struct {
       uint16_t length;
       void*    base;
   } __attribute__((packed)) IDTR = { size, base };

   asm ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

#endif
