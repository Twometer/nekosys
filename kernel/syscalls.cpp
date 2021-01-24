#include <kernel/syscalls.h>
#include <kernel/tasks/thread.h>
#include <kernel/fs/vfs.h>
#include <kernel/tty.h>
#include <stdio.h>

#define SYS_DEBUG 0

using namespace Kernel;

uint32_t sys$$texit(void *param)
{
    uint32_t exit_code = *(uint32_t *)(param);
    printf("User thread %d exited with exit code %d\n", Thread::Current()->GetId(), exit_code);
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
    uint32_t exit_code = *(uint32_t *)(param);
    auto process = Process::Current();
    if (process == nullptr)
        return 1;

    printf("User process %d exited with exit code %d\n", process->GetId(), exit_code);
    process->Kill();
    return 0;
}

uint32_t sys$$putchar(void *param)
{
    char c = *(char *)param;
    TTY::Write(&c, sizeof(c));
    return 0;
}

uint32_t sys$$fopen(void *param)
{
    auto params = (sys$$fopen_param *)param;

    auto file = FS::VirtualFileSystem::GetInstance()->GetFileMeta(params->path);
    if (file.type == FS::DirEntryType::Invalid)
        return 1;

    auto fd = FS::VirtualFileSystem::GetInstance()->Open(params->path);
    *params->fsize = file.size;
    *params->fd = fd;
    return 0;
}

uint32_t sys$$fread(void *param)
{
    auto params = (sys$$fread_param *)param;
    FS::VirtualFileSystem::GetInstance()->Read(params->fd, params->offset, params->length, (uint8_t *)params->dst);
    return 0;
}

uint32_t sys$$fwrite(void *param)
{
    auto params = (sys$$fwrite_param *)param;
    return 0;
}

uint32_t sys$$fclose(void *param)
{
    auto params = (sys$$fclose_param *)param;
    FS::VirtualFileSystem::GetInstance()->Close(params->fd);
    return 0;
}

uint32_t sys$$pagealloc(void *param)
{
    int num = *(int *)param;
    auto ptr = (uint32_t)Process::Current()->MapNewPages(num);
#if SYS_DEBUG
    printf("sys: Mapped %d pages to %x %x\n", num, ptr, Process::Current()->GetHeapBase());
#endif
    return ptr;
}

uint32_t sys$$sleep(void *param)
{
    uint32_t timeout = *(uint32_t *)(param);   
    auto thread = Thread::Current();
    thread->Sleep(timeout * 1000);
}