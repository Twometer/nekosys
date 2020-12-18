#ifndef _HEAP_H
#define _HEAP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void heap_init();

    void *kmalloc(size_t size);

    void kfree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif