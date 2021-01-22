#include <sys/syscall.h>

int liballoc_lock()
{
	return 0;
}

int liballoc_unlock()
{
	return 0;
}

void* liballoc_alloc(int pages)
{
	return (void*) syscall(SYS_PAGEALLOC, &pages);
}

int liballoc_free(void* ptr, int pages)
{
	// TODO
}