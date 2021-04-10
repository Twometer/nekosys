#include <kernel/syscalls.h>
#include <kernel/tasks/thread.h>
#include <kernel/tasks/processdir.h>
#include <kernel/tasks/blockers.h>
#include <kernel/tasks/elfloader.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/device/devicemanager.h>
#include <kernel/video/videomanager.h>
#include <kernel/tasks/elfloader.h>
#include <kernel/environment.h>
#include <kernel/namedpipe.h>
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
    if (!path.IsAbsolute())
    {
        auto pathEnv = Environment::GetInstance()->Get("Path");
        // TODO allow for multiple entries in path

        auto resolvedInPath = nk::Path::Resolve(pathEnv, path);
        auto resolvedInPathStr = resolvedInPath->ToString();
        delete resolvedInPath;
        if (FS::VirtualFileSystem::GetInstance()->GetFileMeta(resolvedInPathStr).type != FS::DirEntryType::Invalid)
            return resolvedInPathStr;
    }

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

    Process::Current()->GetOpenFiles()->Add(fd);
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
    buf->buffer = (uint8_t *)SECONDARY_FRAMEBUFFER_LOC;
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

int sys$$getenv(void *param)
{
    sys$$getenv_param *params = (sys$$getenv_param *)param;
    auto val = Environment::GetInstance()->Get(params->key);
    if (val.Empty())
        return -EINVAL;
    memcpy(params->val, val.CStr(), val.Length());
    return 0;
}

int sys$$readdir(void *param)
{
    sys$$readdir_param *params = (sys$$readdir_param *)param;
    auto dirent = FS::VirtualFileSystem::GetInstance()->GetFileMeta(params->dir);
    if (!dirent.IsValid())
        return -ENOENT;
    if (dirent.type != FS::DirEntryType::Folder)
        return -ENOTDIR;

    auto dir = FS::VirtualFileSystem::GetInstance()->ListDirectory(params->dir);
    size_t maxSize = dir.Size() > params->dstSize ? params->dstSize : dir.Size();
    for (size_t i = 0; i < maxSize; i++)
    {
        auto src = dir[i];
        auto &dst = params->dst[i];
        memcpy(dst.name, src.name.CStr(), src.name.Length() + 1);
        dst.size = src.size;
        dst.type = static_cast<unsigned>(src.type);
    }
    return maxSize;
}

int sys$$shbuf_create(void *param)
{
    size_t size = PARAM_VALUE(param, uint32_t);
    size_t pages = (size + PAGE_SIZE) / PAGE_SIZE;
    // FIXME: Continuous is probably not neccessary
    kdbg("Allocating %d pages for shbuf of size %d\n", pages, size);
    pageframe_t frames = PageManager::GetInstance()->AllocContinuous(pages);
    if (!frames)
        return -ENOMEM;

    auto shbuf = SharedBuffer(frames, pages);
    Process::Current()->GetShbufs()->Add(shbuf);
    return shbuf.GetBufId();
}

int sys$$shbuf_map(void *param)
{
    sys$$shbuf_map_param *params = (sys$$shbuf_map_param *)param;
    kdbg("Mapping shbuf %d\n", params->shbuf);
    SharedBuffer *buf = ProcessDir::GetInstance()->FindShBuf(params->shbuf);
    if (buf == nullptr)
        return -ENOENT;

    auto shbufSize = buf->GetNumPages() * PAGE_SIZE;
    auto currentProcess = Process::Current();
    auto shbufPtr = currentProcess->GetHeapBase();
    currentProcess->SetHeapBase(shbufPtr + shbufSize);

    PageDirectory::Current()->MapRange(buf->GetBaseFrame(), shbufPtr, shbufSize, PAGE_BIT_ALLOW_USER | PAGE_BIT_READ_WRITE);
    *params->dst = shbufPtr;
    return 0;
}

int sys$$shbuf_unmap(void *param)
{
    uint32_t bufid = PARAM_VALUE(param, uint32_t);
    return 0;
}

int sys$$pipe_open(void *param)
{
    nk::String pipeName = (const char *)param;
    return PipeManager::GetInstance()->FindOrCreatePipe(pipeName)->pipeId;
}

int sys$$pipe_close(void *param)
{
    uint32_t pipeId = PARAM_VALUE(param, uint32_t);
    auto pipe = PipeManager::GetInstance()->FindPipe(pipeId);
    if (pipe == nullptr)
        return -ENOENT;
    else if (pipe->ownerProcess == Process::Current()->GetId())
    {
        pipe->broken = true;
        // wait for everyone, and then delete
        return 0;
    }
    return -EPERM; // Only the owner can close
}

int sys$$pipe_recv(void *param)
{
    sys$$pipe_recv_param *params = (sys$$pipe_recv_param *)param;
    auto pipe = PipeManager::GetInstance()->FindPipe(params->pipeId);
    if (pipe == nullptr)
        return -ENOENT;
    else
    {
        // Wait for a packet on the pipe that is for the current process
        Thread::Current()->Block(new PipeBlocker(Process::Current()->GetId(), pipe));

        if (pipe->broken)
        {
            return -EPIPE;
        }

        // Now find that packet, remove it from the queue, and return it to the caller
        for (size_t i = 0; i < pipe->packets.Size(); i++)
        {
            auto packet = pipe->packets.At(i);
            if (packet->dstProcess == Process::Current()->GetId())
            {
                pipe->packets.Remove(i);

                auto size = packet->size > params->size ? params->size : packet->size;

                *params->src = packet->srcProcess;
                memcpy(params->buffer, packet->data, size);

                delete[] packet->data;
                delete packet;

                return size;
            }
        }

        return -EWTF;
    }
}

int sys$$pipe_send(void *param)
{
    sys$$pipe_send_param *params = (sys$$pipe_send_param *)param;

    if (params->size > 16384)
    {
        return -EINVAL;
    }

    auto pipe = PipeManager::GetInstance()->FindPipe(params->pipeId);
    if (pipe == nullptr)
        return -ENOENT;
    else
    {
        auto packet = new PipePacket();
        packet->dstProcess = (params->dstProcess == 0 ? pipe->ownerProcess : params->dstProcess);
        packet->srcProcess = Process::Current()->GetId();
        packet->size = params->size;
        packet->data = new uint8_t[packet->size];
        memcpy(packet->data, params->data, packet->size);
        pipe->packets.Add(packet);
        return 0;
    }
}

int sys$$thread_create(void *param)
{
    auto entryPoint = (ThreadMain)param;
    auto proc = Process::Current();
    auto stack = proc->MapNewPages(1);

    auto thread = Thread::CreateUserThread(entryPoint, proc->GetPageDir(), new Memory::Stack(stack, PAGE_SIZE));
    proc->GetThreads()->Add(thread);
    thread->Start();
    return 0;
}