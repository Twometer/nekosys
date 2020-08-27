#include <kernel/tty.h>
#include <device/keyboard.h>
#include <stdio.h>

#include "arch/interrupts.h"

/* nekosys Kernel entry point */
void nkmain() {
	// Welcome message
	tty_clear();
	tty_setcolor(0x0b);
	printf("nekosys 0.01 <by Twometer>\n");
	tty_setcolor(0x0f);

	// Set up environment
	printf("Setting up interrupts...\n");
	int_disable();
	idt_init();
	int_enable();

	printf("Registering devices...\n");
	kbd_initialize();

	// Dummy terminal
	printf("Initialized.\n\n");
	printf("$ ");

	for(;;) {
	 asm("hlt");
	}
}
