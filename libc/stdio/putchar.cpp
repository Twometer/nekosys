#include <kernel/tty.h>
#include <stdio.h>

extern "C"
{

	int putchar(int ic)
	{
		char c = (char)ic;
		Kernel::TTY::Write(&c, sizeof(c));
		return ic;
	}
}