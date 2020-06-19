#include <stdio.h>

#include "gdt.h"

extern void setGdt(uint8_t*, int);

void gdt_initialize() {
/*  GDT[0] = (GDT_entry){.base=0, .limit=0, .type=0};                      // Selector 0x00 cannot be used
  GDT[1] = (GDT_entry){.base=0x04000000, .limit=0x03ffffff, .type=0x9A}; // Selector 0x08 will be our code
  GDT[2] = (GDT_entry){.base=0x08000000, .limit=0x03ffffff, .type=0x92}; // Selector 0x10 will be our data
  //table[3] = (GDT_entry){.base=&myTss, .limit=sizeof(myTss), .type=0x89};  // You can use LTR(0x18)
  */
  uint8_t gdt[3*8];
  gdt_encode_entry(gdt+0x00, (GDT_entry){.base=0, .limit=0, .type=0}); // Selector 0x00 cannot be used
  gdt_encode_entry(gdt+0x08, (GDT_entry){.base=0x04000000, .limit=0x03ffffff, .type=0x9A}); // Selector 0x08 will be our code
  gdt_encode_entry(gdt+0x10, (GDT_entry){.base=0x08000000, .limit=0x03ffffff, .type=0x92}); // Selector 0x10 will be our data

  setGdt(gdt, sizeof(gdt));
}

void gdt_encode_entry(uint8_t *target, GDT_entry source) {
  // Check the limit to make sure that it can be encoded
  if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
      printf("Invalid GDT parameters");
      return;
  }
  if (source.limit > 65536) {
      // Adjust granularity if required
      source.limit = source.limit >> 12;
      target[6] = 0xC0;
  } else {
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
