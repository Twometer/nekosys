#include <stdio.h>

#ifdef __KERNEL
#include <kernel/video/tty.h>
#else
#include <sys/syscall.h>
#endif

extern "C"
{

	int putchar(int ic)
	{
		char c = (char)ic;
#ifdef __KERNEL
		Kernel::TTY::Write(&c, sizeof(c));
#else
		syscall(SYS_PUTCHAR, &c);
#endif
		return ic;
	}
}