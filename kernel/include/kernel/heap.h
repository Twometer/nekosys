#include <stddef.h>
#include <stdint.h>

void heap_init();

void* malloc(size_t size);

void free(void *ptr);
