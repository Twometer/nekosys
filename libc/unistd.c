#include <sys/syscall.h>
#include <unistd.h>

unsigned int usleep(unsigned int microseconds)
{
    microseconds /= 1000;                   // Sleep takes ms
    return syscall(SYS_SLEEP, &microseconds);
}

unsigned int sleep(unsigned int seconds)
{
    seconds *= 1000;
    return syscall(SYS_SLEEP, &seconds);    // Sleep takes ms
}

int chdir(const char *path)
{
    return syscall(SYS_CHDIR, path);
}

char *getcwd(char *buf, size_t size)
{
    sys$$getcwd_param param;
    param.buf = buf;
    param.size = size;
    if (syscall(SYS_GETCWD, &param))
        return NULL;
    else
        return buf;
}