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

int shbuf_create(size_t size)
{
    return syscall(SYS_SHBUFCREATE, &size);
}

int shbuf_map(int bufid, void **mapped)
{
    sys$$shbuf_map_param param;
    param.shbuf = bufid;
    param.dst = mapped;
    return syscall(SYS_SHBUFMAP, &param);
}

int shbuf_unmap(int bufid)
{
}

int thread_create(void (*entryPoint)())
{
    return syscall(SYS_THCREATE, entryPoint);
}

int thread_die(int retcode)
{
    return syscall(SYS_TEXIT, &retcode);
}

int pipe_open(const char *name)
{
    return syscall(SYS_PIPEOPEN, name);
}

int pipe_close(int pipeid)
{
    return syscall(SYS_PIPECLOSE, &pipeid);
}

int pipe_recv(int pipeid, pid_t *src, size_t size, uint8_t *buffer)
{
    sys$$pipe_recv_param param;
    param.pipeId = pipeid;
    param.src = src;
    param.size = size;
    param.buffer = buffer;
    return syscall(SYS_PIPERECV, &param);
}

int pipe_send(int pipeid, size_t size, uint8_t *data)
{
    sys$$pipe_send_param param;
    param.pipeId = pipeid;
    param.size = size;
    param.data = data;
    return syscall(SYS_PIPESEND, &param);
}