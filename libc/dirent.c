#include <sys/syscall.h>
#include <dirent.h>

DIR *opendir(const char *name)
{
    DIR *dir = malloc(sizeof(DIR));
    dir->idx = 0;

    sys$$readdir_param param;
    param.dir = name;
    param.dst = dir->data;
    param.dstSize = 256;
    int result = syscall(SYS_READDIR, &param);
    if (result < 0)
    {
        free(dir);
        return NULL;
    }
    dir->max = result;
    return dir;
}

int closedir(DIR *dir)
{
    free(dir);
}

dirent *readdir(DIR *dir)
{
    if (dir->idx >= dir->max)
        return NULL;
    dirent *d = &dir->data[dir->idx];
    dir->idx++;
    return d;
}