#include <sys/syscall.h>
#include <nekosys.h>

int spawnp(pid_t *pid, const char *path, char *const argv[], char *const env[])
{
    sys$$spawnp_param param;
    param.pid = pid;
    param.path = path;
    param.argv = (char**) argv;
    param.env = (char**) env;
    syscall(SYS_SPAWNP, &param);
}

int waitp(pid_t pid)
{
    syscall(SYS_WAITP, &pid);
}