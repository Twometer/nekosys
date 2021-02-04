#include <sys/syscall.h>
#include <stdlib.h>

char *getenv(const char *name)
{
    char *data = malloc(512);
    sys$$getenv_param param;
    param.key = name;
    param.val = data;

    if (syscall(SYS_GETENV, &param))
    {
        free(data);
        return NULL;
    }

    return data;
}