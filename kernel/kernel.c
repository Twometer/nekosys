#include <kernel/tty.h>
#include <stdio.h>

#include "arch/interrupts.h"

/* nekosys Kernel entry point */
void nkmain() {
	// Welcome message
	tty_clear();
	printf("nekosys 0.01 <by Twometer>\n");

	// Set up environment
	printf("Setting up interrupts...\n");

	int_disable();
	idt_init();
	int_enable();

	// Dummy terminal
	printf("Initialized.\n\n");
	printf("$ ");

	for(;;) {
	 asm("hlt");
	}
}
