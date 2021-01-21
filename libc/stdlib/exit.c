#include <sys/syscall.h>
#include <stdlib.h>

void exit(int retval)
{
    syscall(SYS_EXIT, &retval);
}