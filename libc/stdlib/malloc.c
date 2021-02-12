#include <stdlib.h>
#ifdef __KERNEL
#include <kernel/memory/heap.h>
#else
#include "../liballoc/liballoc.h"
#endif

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

void *realloc(void *ptr, size_t size)
{
#ifndef __KERNEL
    return usr_realloc(ptr, size);
#endif
}