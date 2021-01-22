#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <kernel/syscalls.h>
#include <stdint.h>

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