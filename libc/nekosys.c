#include <sys/syscall.h>
#include <nekosys.h>

int spawnp(pid_t *pid, const char *path, char *const argv[], char *const env[])
{
    sys$$spawnp_param param;
    param.pid = pid;
    param.path = path;
    param.argv = (char **)argv;
    param.env = (char **)env;
    return syscall(SYS_SPAWNP, &param);
}

int waitp(pid_t pid)
{
    return syscall(SYS_WAITP, &pid);
}

int readln(char *dst, size_t maxSize)
{
    sys$$readln_param param;
    param.dst = dst;
    param.maxSize = maxSize;
    return syscall(SYS_READLN, &param);
}