#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct IDT_entry {
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
} __attribute__ ((packed)) IDT_entry;

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

static inline bool are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

#define PIC_EOI		0x20		/* End-of-interrupt command code */
 
void PIC_sendEOI(unsigned char irq);
void PIC_remap(int opffset1, int offset2);

#endif
