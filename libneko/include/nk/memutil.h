#ifndef _MEMUTIL_H
#define _MEMUTIL_H

#include <stdint.h>
#include <stddef.h>

void fast_copy(void *src, void *dst, size_t size)
{
    size_t num_dwords = size / sizeof(uint32_t);
    size_t num_bytes = size % sizeof(uint32_t);

    uint32_t *wdst = (uint32_t *)dst;
    uint32_t *wsrc = (uint32_t *)src;
    for (size_t i = 0; i < num_dwords; i++)
        wdst[i] = wsrc[i];

    uint8_t *bdst = (uint8_t *)(wdst + num_dwords);
    uint8_t *bsrc = (uint8_t *)(wsrc + num_dwords);
    for (size_t i = 0; i < num_bytes; i++)
        bdst[i] = bsrc[i];
}

#endif