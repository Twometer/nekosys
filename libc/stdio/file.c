#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>

FILE *fopen(const char *path, const char *mode)
{
    FILE *file = malloc(sizeof(FILE));
    sys$$fopen_param param;
    param.path = path;
    param.mode = mode;
    param.fd = &file->fd;
    param.fsize = &file->fsize;
    syscall(SYS_FOPEN, &param);
    return file;
}

int fclose(FILE *stream)
{
    sys$$fclose_param param;
    param.fd = stream->fd;
    free(stream);
    return syscall(SYS_FCLOSE, &param);
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    size_t num_bytes = size * count;
    sys$$fread_param param;
    param.fd = stream->fd;
    param.dst = ptr;
    param.offset = stream->offset;
    param.length = num_bytes;
    return syscall(SYS_FREAD, &param);
}

int fseek(FILE *stream, long offset, int origin)
{
    if (offset < 0 || offset >= stream->fsize)
        return 1;

    if (origin == SEEK_SET)
        stream->offset = offset;
    else if (origin == SEEK_CUR)
        stream->offset += offset;
    else if (origin == SEEK_END)
        stream->offset = stream->fsize - offset;
    return 0;
}

long ftell(FILE *stream)
{
    return stream->offset;
}