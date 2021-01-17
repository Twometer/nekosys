#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <stdint.h>

#define SYS_INVAL 0
#define SYS_TEXIT 1
#define SYS_PRINT 2

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct syscall_invoke
    {
        uint32_t number;
        const void *params;
        uint32_t retval;
    } syscall_invoke_t;

    uint32_t syscall(uint32_t number, const void *params);

#ifdef __cplusplus
}
#endif

#endif