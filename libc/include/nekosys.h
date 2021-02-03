/*
 * Header for custom libc functions that I don't want to 
 * put into the default headers as not to confuse any
 * software that might use them sometime in the future
 */

#ifndef _NEKOSYS_H
#define _NEKOSYS_H

#define PATH_MAX 1024

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        void *buffer;
        size_t width;
        size_t height;
        size_t pitch;
        size_t pixelStride;
    } FRAMEBUF;

    int spawnp(pid_t *pid, const char *path, int argc, const char **argv);
    int waitp(pid_t pid);

    int readln(char *dst, size_t maxSize);

    int framebuf_acquire(FRAMEBUF *framebuf);
    int framebuf_flush(int x, int y, int w, int h);
    int framebuf_flush_all();
    int framebuf_release();

#ifdef __cplusplus
}
#endif

#endif