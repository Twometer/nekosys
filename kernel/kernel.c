#include <kernel/tty.h>
#include <stdio.h>

#include "interrupts.h"

/* nekosys Kernel entry point */
void nkmain() {
	tty_clear();
	printf("nekosys Kernel 0.01\n");
	printf("(c) 2020 Twometer Applications\n");

	interrupts_register();
}
