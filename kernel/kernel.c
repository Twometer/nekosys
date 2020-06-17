#include <kernel/tty.h>

/* nekosys Kernel entry point */
void nkmain() {
	tty_clear();
	tty_print("nekosys Kernel 0.01\n");
}
