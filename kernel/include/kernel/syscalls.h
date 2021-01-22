#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

#define SYS_INVAL 0
#define SYS_TEXIT 1
#define SYS_PRINT 2
#define SYS_EXIT 3
#define SYS_PUTCHAR 4
#define SYS_FOPEN 5
#define SYS_FREAD 6
#define SYS_FWRITE 7
#define SYS_FCLOSE 8

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        const char *path;
        const char *mode;
        int *fd;
        long *fsize;
    } sys$$fopen_param;

    typedef struct
    {
        int fd;
        void *dst;
        size_t offset;
        size_t length;
    } sys$$fread_param;

    typedef struct
    {
        int fd;
    } sys$$fwrite_param;

    typedef struct
    {
        int fd;
    } sys$$fclose_param;

#ifdef __cplusplus
}
#endif

uint32_t sys$$texit(void *param);
uint32_t sys$$print(void *param);
uint32_t sys$$exit(void *param);
uint32_t sys$$putchar(void *param);
uint32_t sys$$fopen(void *param);
uint32_t sys$$fread(void *param);
uint32_t sys$$fwrite(void *param);
uint32_t sys$$fclose(void *param);

#endif