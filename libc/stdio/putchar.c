#include <kernel/tty.h>
#include <stdio.h>

int putchar(int ic) {
	char c = (char) ic;
	tty_write(&c, sizeof(c));
	return ic;
}
