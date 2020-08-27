#include <kernel/tty.h>
#include <stdio.h>

#include "arch/interrupts.h"

/* nekosys Kernel entry point */
void nkmain() {
	// Welcome message
	tty_clear();
	printf("nekosys Kernel 0.01\n");
	printf("(c) 2020 Twometer Applications\n");

	// Set up environment
	//idt_init();
	//int_enable();

	// Dummy terminal
	printf("Init completed\n\n");
	printf("root@neko $ ");
}
