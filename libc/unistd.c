#include <sys/syscall.h>
#include <unistd.h>

unsigned int sleep(unsigned int seconds)
{
    return syscall(SYS_SLEEP, &seconds);
}