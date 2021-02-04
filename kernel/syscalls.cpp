#include <kernel/syscalls.h>
#include <kernel/tasks/thread.h>
#include <kernel/tasks/processdir.h>
#include <kernel/tasks/blockers.h>
#include <kernel/tasks/elfloader.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/device/devicemanager.h>
#include <kernel/video/videomanager.h>
#include <kernel/kdebug.h>
#include <kernel/fs/vfs.h>
#include <nekosys.h>
#include <stdio.h>
#include <errno.h>

#include <nk/path.h>

using namespace Kernel;
using namespace Memory;
using namespace Video;

nk::String resolve_file(const nk::Path path)
{
    auto resolved = nk::Path::Resolve(Process::Current()->GetCwd(), path);
    auto file = resolved->ToString();
    delete resolved;
    return file;
}

int sys$$texit(void *param)
{
    uint32_t exit_code = *(uint32_t *)(param);
    kdbg("User thread %d exited with exit code %d\n", Thread::Current()->GetId(), exit_code);
    Thread::Current()->Kill();
    return 0;
}

int sys$$print(void *param)
{
    printf("%s", param);
    return 0;
}

int sys$$exit(void *param)
{
    uint32_t exit_code = *(uint32_t *)(param);
    auto process = Process::Current();
    if (process == nullptr)
        return -ENOSYS;

    process->Exit(exit_code);
    return 0;
}

int sys$$putchar(void *param)
{
    char c = *(char *)param;
    VideoManager::GetInstance()->GetTTY()->Write(&c, sizeof(c));
    return 0;
}

int sys$$fopen(void *param)
{
    auto params = (sys$$fopen_param *)param;

    auto resolved = resolve_file(params->path);
    auto file = FS::VirtualFileSystem::GetInstance()->GetFileMeta(resolved);
    if (file.type == FS::DirEntryType::Invalid)
        return -ENOENT;

    auto fd = FS::VirtualFileSystem::GetInstance()->Open(resolved);
    *params->fsize = file.size;
    *params->fd = fd;
    return 0;
}

int sys$$fread(void *param)
{
    auto params = (sys$$fread_param *)param;
    FS::VirtualFileSystem::GetInstance()->Read(params->fd, params->offset, params->length, (uint8_t *)params->dst);
    return 0;
}

int sys$$fwrite(void *param)
{
    auto params = (sys$$fwrite_param *)param;
    return 0;
}

int sys$$fclose(void *param)
{
    auto params = (sys$$fclose_param *)param;
    FS::VirtualFileSystem::GetInstance()->Close(params->fd);
    return 0;
}

int sys$$pagealloc(void *param)
{
    int num = *(int *)param;
    auto ptr = (uint32_t)Process::Current()->MapNewPages(num);
#if SYS_DEBUG
    kdbg("sys: Mapped %d pages to %x %x\n", num, ptr, Process::Current()->GetHeapBase());
#endif
    return ptr;
}

int sys$$sleep(void *param)
{
    uint32_t timeout = *(uint32_t *)(param);
    auto thread = Thread::Current();
    thread->Sleep(timeout * 1000);
    return 0;
}

int sys$$spawnp(void *param)
{
    auto params = (sys$$spawnp_param *)param;
    auto resolved = resolve_file(params->path);
#if SYS_DEBUG
    kdbg("sys: spawnp: argc=%d argv=%x\n", params->argc, params->argv);
#endif
    auto proc = ElfLoader::CreateProcess(resolved.CStr(), params->argc, params->argv);
    if (proc == nullptr)
        return -ENOEXEC;
    *params->pid = proc->GetId();
    proc->SetCwd(Process::Current()->GetCwd());
    proc->Start();
    return 0;
}

int sys$$waitp(void *param)
{
    pid_t pid = PARAM_VALUE(param, pid_t);
    auto proc = ProcessDir::GetInstance()->GetProcess(pid);
    if (proc == nullptr)
        return -ESRCH;
    Thread::Current()->Block(new ProcessWaitBlocker(proc));
    return proc->GetExitCode();
}

int sys$$readln(void *param)
{
    auto params = (sys$$readln_param *)param;
    Thread::Current()->Block(new KeyboardBlocker('\n'));
    return Device::DeviceManager::keyboard->ReadUntil(params->dst, params->maxSize, '\n');
}

int sys$$fb_acquire(void *param)
{
    auto *buf = (FRAMEBUF *)param;
    auto vm = VideoManager::GetInstance();
    if (!vm->KernelControlsFramebuffer())
    {
        // User processes cannot steal the fbuf from each other
        return -EPERM;
    }
    vm->AcquireFramebuffer(Process::Current()->GetId());
    PageDirectory::Current()->MapRange(vm->GetFramebufferPhysical(), (vaddress_t)SECONDARY_FRAMEBUFFER_LOC, vm->GetFramebufferSize(), PAGE_BIT_READ_WRITE | PAGE_BIT_ALLOW_USER);
    buf->buffer = (void *)SECONDARY_FRAMEBUFFER_LOC;
    buf->width = vm->GetCurrentMode()->Xres;
    buf->height = vm->GetCurrentMode()->Yres;
    buf->pitch = vm->GetCurrentMode()->pitch;
    buf->pixelStride = vm->GetPixelStride();
    return 0;
}

int sys$$fb_flush(void *param)
{
    auto params = (sys$$fb_flush_param *)param;

    auto vm = VideoManager::GetInstance();
    if (vm->GetFramebufferController() != Process::Current()->GetId())
        return -EACCES;

    if (params->full)
        vm->FlushBuffer();
    else
        vm->FlushBlock(params->x, params->y, params->w, params->h);

    return 0;
}

int sys$$fb_release(void *)
{
    auto vm = VideoManager::GetInstance();
    if (vm->GetFramebufferController() != Process::Current()->GetId())
        return -EACCES;

    // TODO: Unmap the fbuf

    vm->AcquireFramebuffer(0);
    return 0;
}

int sys$$chdir(void *param)
{
    char *params = (char *)param;
    auto newCwd = nk::Path::Resolve(Process::Current()->GetCwd(), params);

    auto dirent = FS::VirtualFileSystem::GetInstance()->GetFileMeta(newCwd->ToString());
    if (dirent.type != FS::DirEntryType::Folder)
        return -ENOTDIR;

    Process::Current()->SetCwd(newCwd->ToString());
    delete newCwd;
    return 0;
}

int sys$$getcwd(void *param)
{
    sys$$getcwd_param *params = (sys$$getcwd_param *)param;

    auto cwd = Process::Current()->GetCwd();
    auto size = params->size;
    if (size > cwd.Length())
        size = cwd.Length() + 1;

    memcpy(params->buf, cwd.CStr(), size);
    return 0;
}