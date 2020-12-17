#include <kernel/tty.h>

extern "C"
{
#include <stdio.h>

	int putchar(int ic)
	{
		char c = (char)ic;
		Kernel::TTY::Write(&c, sizeof(c));
		return ic;
	}
}