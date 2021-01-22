#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stdint.h>

#define SYS_INVAL 0
#define SYS_TEXIT 1
#define SYS_PRINT 2
#define SYS_EXIT 3
#define SYS_PUTCHAR 4

uint32_t sys$$texit(void *param);
uint32_t sys$$print(void *param);
uint32_t sys$$exit(void *param);
uint32_t sys$$putchar(void *param);

#endif