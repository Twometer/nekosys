#include <kernel/syscalls.h>
#include <kernel/tasks/thread.h>
#include <kernel/tty.h>
#include <stdio.h>

using namespace Kernel;

uint32_t sys$$texit(void *param)
{
    uint32_t exit_code = *(uint32_t *)(param);
    printf("UserThread %d exited with exit code %d\n", Thread::Current()->GetId(), exit_code);
    Thread::Current()->Kill();
    return 0;
}

uint32_t sys$$print(void *param)
{
    printf("%s", param);
    return 0;
}

uint32_t sys$$exit(void *param)
{
    printf("User process exited\n");
    sys$$texit(param);
    return 0;
}

uint32_t sys$$putchar(void *param)
{
    char c = *(char *)param;
    TTY::Write(&c, sizeof(c));
    return 0;
}
