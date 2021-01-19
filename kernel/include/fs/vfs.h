#ifndef _VIRTUAL_FILE_SYSTEM_H
#define _VIRTUAL_FILE_SYSTEM_H

#include <stdint.h>
#include <nk/vector.h>
#include <nk/string.h>
#include <fs/mountpoint.h>
#include <fs/file.h>

namespace FS
{
    class VirtualFileSystem
    {
    private:
        nk::Vector<MountPoint *> mounts;

    public:
        void Mount(const nk::String &path, FileSystem *fs);

        void Unmount(const nk::String &path);

        bool Exists(const nk::String &path);

        File *GetFile(const nk::String &path);

        uint8_t *ReadFile(File *file);

        void ListDirectory(const nk::String &path);

    private:
        MountPoint *FindMountPoint(const nk::String &path);
    };
} // namespace FS

#endif