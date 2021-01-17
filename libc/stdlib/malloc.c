#include <stdlib.h>
#include <kernel/heap.h>

// TODO userspace allocator

void *malloc(size_t size)
{
#ifdef __KERNEL
    return kmalloc(size);
#else

#endif
}

void free(void *ptr)
{
#ifdef __KERNEL
    return kfree(ptr);
#else

#endif
}