#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>
#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

uint8_t *read_file(const char *path, size_t *size);

#endif