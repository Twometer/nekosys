#include <fs/vfs.h>

#define VFS_DEBUG 0

namespace FS
{

    void VirtualFileSystem::Mount(const nk::String &path, FileSystem *fs)
    {
        mounts.Add(new MountPoint(path, fs));
    }

    void VirtualFileSystem::Unmount(const nk::String &path)
    {
        for (size_t i = 0; i < mounts.Size(); i++)
        {
            if (mounts.At(i)->path == path)
            {
                auto mount = mounts.At(i);
                mounts.Remove(i);
                delete mount;
                break;
            }
        }
    }

    bool VirtualFileSystem::Exists(const nk::String &path)
    {
        auto mountPoint = FindMountPoint(path);
        auto subPath = path.Substring(mountPoint->path.Length());
        return mountPoint->fs->Exists(path);
    }

    DirEntry VirtualFileSystem::GetFileMeta(const nk::String &path)
    {
        auto mountPoint = FindMountPoint(path);
        if (mountPoint == nullptr)
        {
            printf("vfs: Mount point not found for %s\n", path.CStr());
            return DirEntry::Invalid;
        }
#if VFS_DEBUG
        printf("vfs: get_file_meta %s\n", mountPoint->path.CStr());
#endif
        return mountPoint->fs->GetFileMeta(GetRelativePath(mountPoint, path));
    }

    uint32_t VirtualFileSystem::Open(const nk::String &path)
    {
    }

    void VirtualFileSystem::Read(uint32_t fileHandle, size_t size, uint8_t *dst)
    {
    }

    void VirtualFileSystem::Close(uint32_t fileHandle)
    {
    }

    void VirtualFileSystem::ListDirectory(const nk::String &path)
    {
        auto mountPoint = FindMountPoint(path);
        if (mountPoint == nullptr)
        {
            printf("vfs: Mount point not found for %s\n", path.CStr());
            return;
        }
#if VFS_DEBUG
        printf("vfs: list_dir %s\n", mountPoint->path.CStr());
#endif
        mountPoint->fs->ListDirectory(GetRelativePath(mountPoint, path));
    }

    nk::String VirtualFileSystem::GetRelativePath(MountPoint *mountPoint, const nk::String &absolutePath)
    {
        nk::String base = "/";
        nk::String relative = absolutePath.Substring(mountPoint->path.Length());
        return base.Append(relative);
    }

    MountPoint *VirtualFileSystem::FindMountPoint(const nk::String &path)
    {
        for (size_t i = 0; i < mounts.Size(); i++)
        {
            auto mount = mounts.At(i);
            if (path.StartsWith(mount->path))
                return mount;
        }
        return nullptr;
    }

} // namespace FS