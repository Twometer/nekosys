#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <dirent.h>

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
#define SYS_GETENV 19
#define SYS_READDIR 20
#define SYS_SHBUFCREATE 21
#define SYS_SHBUFMAP 22
#define SYS_SHBUFUNMAP 23
#define SYS_PIPEOPEN 24
#define SYS_PIPECLOSE 25
#define SYS_PIPERECV 26
#define SYS_PIPESEND 27

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
        int argc;
        char **argv;
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

    typedef struct
    {
        char *key;
        char *val;
    } sys$$getenv_param;

    typedef struct
    {
        const char *dir;
        dirent *dst;
        size_t dstSize;
    } sys$$readdir_param;

    typedef struct
    {
        int shbuf;
        void **dst;
    } sys$$shbuf_map_param;

    typedef struct
    {
        int pipeId;
        pid_t *src;
        size_t size;
        uint8_t *buffer;
    } sys$$pipe_recv_param;

    typedef struct
    {
        int pipeId;
        size_t size;
        uint8_t *data;
    } sys$$pipe_send_param;

#ifdef __cplusplus
}
#endif

int sys$$texit(void *param);
int sys$$print(void *param);
int sys$$exit(void *param);
int sys$$putchar(void *param);
int sys$$fopen(void *param);
int sys$$fread(void *param);
int sys$$fwrite(void *param);
int sys$$fclose(void *param);
int sys$$pagealloc(void *param);
int sys$$sleep(void *param);
int sys$$spawnp(void *param);
int sys$$waitp(void *param);
int sys$$readln(void *param);
int sys$$fb_acquire(void *param);
int sys$$fb_flush(void *param);
int sys$$fb_release(void *param);
int sys$$chdir(void *param);
int sys$$getcwd(void *param);
int sys$$getenv(void *param);
int sys$$readdir(void *param);
int sys$$shbuf_create(void *param);
int sys$$shbuf_map(void *param);
int sys$$shbuf_unmap(void *param);
int sys$$pipe_open(void *param);
int sys$$pipe_close(void *param);
int sys$$pipe_recv(void *param);
int sys$$pipe_send(void *param);

#endif
