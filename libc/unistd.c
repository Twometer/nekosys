#include <sys/syscall.h>
#include <unistd.h>

unsigned int sleep(unsigned int seconds)
{
    return syscall(SYS_SLEEP, &seconds);
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