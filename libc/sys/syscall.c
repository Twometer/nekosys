#include <sys/syscall.h>

uint32_t syscall(uint32_t number, const void *params)
{
    syscall_invoke_t invoke;
    invoke.number = number;
    invoke.params = params;
    invoke.retval = 0;
    asm volatile("int $0x80");
    return invoke.retval;
}