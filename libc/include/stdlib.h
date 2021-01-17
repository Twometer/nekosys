#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    __attribute__((__noreturn__)) void abort(void);
    __attribute__((__noreturn__)) void exit(int retval);
    void *malloc(size_t size);
    void free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
