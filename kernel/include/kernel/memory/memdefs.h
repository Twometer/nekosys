#ifndef _MEMDEFS_H
#define _MEMDEFS_H

#include <stdint.h>

typedef uint8_t *pageframe_t;
typedef uint8_t *vaddress_t;
typedef uint8_t *paddress_t;

// General bytes stuff
#define KBYTE 1024
#define MBYTE 1024 * KBYTE
#define GBYTE 1024 * MBYTE

// Paging
#define PAGE_SIZE 0x1000
#define PAGE_ALIGN_DOWN(addr) (uint8_t *)((uint32_t)(addr) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (uint8_t *)(((uint32_t)(addr) + PAGE_SIZE) & ~(PAGE_SIZE - 1))
#define IS_PAGE_ALIGNED(addr) (PAGE_ALIGN_DOWN(addr) == (uint8_t *)addr)

#define PAGE_BIT_ALLOW_USER (1 << 2)
#define PAGE_BIT_READ_WRITE (1 << 1)
#define PAGE_BIT_PRESENT (1 << 0)

#define PAGE_USED 0x01
#define PAGE_FREE 0x00

// Kernel Heap
#define KERNEL_HEAP_ADDR 0xC0000000
#define KERNEL_HEAP_SIZE 1 * MBYTE

// Other memory constants
#define PAGE_ALLOC_REMAP 0xFF000000
#define PAGE_DIR_TEMP_LOC 0xFF100000
#define PAGETABLES_VIRTUAL_LOC 0xFFC00000
#define PAGEDIR_VIRTUAL_LOC 0xFFFFF000

// GDT
#define SEG_NULL 0x00
#define SEG_KRNL_CODE 0x08
#define SEG_KRNL_DATA 0x10
#define SEG_USER_CODE 0x18
#define SEG_USER_DATA 0x20
#define SEG_TSS 0x28

#endif