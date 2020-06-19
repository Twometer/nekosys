#include "interrupts.h"

void interrupts_initialize() {
  //IDT_entry idt[1];
  //idt[0] = (IDT_entry){.selector=0x08, .zero=0,}
  // TODO Setup IDT, register handlers etc.
}

void interrupts_disable() {
  asm("cli");
}

void interrupts_enable() {
  asm("sti");
}
