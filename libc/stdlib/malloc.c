#include <stdlib.h>
#ifdef __KERNEL
#include <kernel/heap.h>
#else
#include "../liballoc/liballoc.h"
#endif

// TODO userspace allocator

void *malloc(size_t size)
{
#ifdef __KERNEL
    return kmalloc(size);
#else
    return usr_malloc(size);
#endif
}

void free(void *ptr)
{
#ifdef __KERNEL
    return kfree(ptr);
#else
    return usr_free(ptr);
#endif
}