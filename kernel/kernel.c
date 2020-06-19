#include <kernel/tty.h>
#include <stdio.h>

#include "interrupts.h"
#include "gdt.h"

/* nekosys Kernel entry point */
void nkmain() {
	tty_clear();
	printf("nekosys Kernel 0.01\n");
	printf("(c) 2020 Twometer Applications\n");

	interrupts_disable();
	gdt_initialize();
	interrupts_initialize();
	interrupts_enable();

	printf(" Init completed\n");
	printf("\n");

	printf("root@neko $ ");

	for(;;) {
	 asm("hlt");
	}
}
