#ifndef _MEMDEFS_H
#define _MEMDEFS_H

#include <stdint.h>

typedef uint8_t *pageframe_t;
typedef uint8_t *vaddress_t;
typedef uint8_t *paddress_t;

#define KBYTE 1024
#define MBYTE 1024 * KBYTE
#define GBYTE 1024 * MBYTE

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN_DOWN(addr) (uint8_t *)((uint32_t)(addr) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (uint8_t *)(((uint32_t)(addr) + PAGE_SIZE) & ~(PAGE_SIZE - 1))
#define IS_PAGE_ALIGNED(addr) (PAGE_ALIGN_DOWN(addr) == (uint8_t *)addr)

#define PAGE_BIT_ALLOW_USER (1 << 2)
#define PAGE_BIT_READ_WRITE (1 << 1)
#define PAGE_BIT_PRESENT (1 << 0)

#define PAGE_USED 0x01
#define PAGE_FREE 0x00

#endif