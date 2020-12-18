#include <stdlib.h>
#include <kernel/heap.h>

// TODO thats not how it works lol

void *malloc(size_t size)
{
    return kmalloc(size);
}

void free(void *ptr)
{
    return kfree(ptr);
}