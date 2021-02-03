#include <sys/syscall.h>
#include <nekosys.h>

int spawnp(pid_t *pid, const char *path, int argc, const char **argv)
{
    sys$$spawnp_param param;
    param.pid = pid;
    param.path = path;
    param.argc = argc;
    param.argv = (char **)argv;
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

int framebuf_acquire(FRAMEBUF *framebuf)
{
    return syscall(SYS_FBACQUIRE, framebuf);
}

int framebuf_flush(int x, int y, int w, int h)
{
    sys$$fb_flush_param param;
    param.full = 0;
    param.x = x;
    param.y = y;
    param.w = w;
    param.h = h;
    return syscall(SYS_FBFLUSH, &param);
}

int framebuf_flush_all()
{
    sys$$fb_flush_param param;
    param.full = 1;
    return syscall(SYS_FBFLUSH, &param);
}

int framebuf_release()
{
    return syscall(SYS_FBRELEASE, 0);
}