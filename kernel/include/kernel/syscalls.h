#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/types.h>
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
#define SYS_PAGEALLOC 9
#define SYS_SLEEP 10
#define SYS_SPAWNP 11
#define SYS_WAITP 12
#define SYS_READLN 13
#define SYS_FBACQUIRE 14
#define SYS_FBFLUSH 15
#define SYS_FBRELEASE 16
#define SYS_CHDIR 17
#define SYS_GETCWD 18

#define PARAM_VALUE(param, type) (*(type *)(param))

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

    typedef struct
    {
        pid_t *pid;
        const char *path;
        char **argv;
        char **env;
    } sys$$spawnp_param;

    typedef struct
    {
        char *dst;
        size_t maxSize;
    } sys$$readln_param;

    typedef struct
    {
        int full;
        int x;
        int y;
        int w;
        int h;
    } sys$$fb_flush_param;

    typedef struct
    {
        char *buf;
        size_t size;
    } sys$$getcwd_param;

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
uint32_t sys$$pagealloc(void *param);
uint32_t sys$$sleep(void *param);
uint32_t sys$$spawnp(void *param);
uint32_t sys$$waitp(void *param);
uint32_t sys$$readln(void *param);
uint32_t sys$$fb_acquire(void *param);
uint32_t sys$$fb_flush(void *param);
uint32_t sys$$fb_release(void *param);
uint32_t sys$$chdir(void *param);
uint32_t sys$$getcwd(void *param);

#endif
