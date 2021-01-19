#include <fs/vfs.h>

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

    File *VirtualFileSystem::GetFile(const nk::String &path)
    {
        auto mountPoint = FindMountPoint(path);
        auto subPath = path.Substring(mountPoint->path.Length());
        return mountPoint->fs->GetFile(subPath);
    }

    uint8_t *VirtualFileSystem::ReadFile(File *file)
    {
        // TODO this api is shit
        /*auto mountPoint = FindMountPoint(file->path);
        auto subPath = file->path.Substring(mountPoint->path.Length());
        mountPoint->fs->ReadFile(sub)*/
    }

    void VirtualFileSystem::ListDirectory(const nk::String &path)
    {
        auto mountPoint = FindMountPoint(path);
        auto subPath = path.Substring(mountPoint->path.Length()).Append("/");
        mountPoint->fs->ListDirectory(subPath);
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