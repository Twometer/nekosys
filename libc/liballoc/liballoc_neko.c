#include <sys/syscall.h>
#include <stdio.h>

int liballoc_lock()
{
	return 0;
}

int liballoc_unlock()
{
	return 0;
}

void *liballoc_alloc(int pages)
{
	int page = syscall(SYS_PAGEALLOC, &pages);
	return (void *)page;
}

int liballoc_free(void *ptr, int pages)
{
	// TODO
}