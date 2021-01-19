#ifndef _VIRTUAL_FILE_SYSTEM_H
#define _VIRTUAL_FILE_SYSTEM_H

#include <stdint.h>
#include <nk/vector.h>
#include <nk/string.h>
#include <fs/mountpoint.h>
#include <fs/direntry.h>
#include <fs/filehandle.h>

namespace FS
{
    class VirtualFileSystem
    {
    private:
        nk::Vector<MountPoint *> mounts;

        nk::Vector<FileHandle *> fileHandles;

        uint32_t idCounter;
        

    public:
        void Mount(const nk::String &path, FileSystem *fs);

        void Unmount(const nk::String &path);

        bool Exists(const nk::String &path);

        DirEntry GetFileMeta(const nk::String &path);

        uint32_t Open(const nk::String &path);

        void Read(uint32_t fileHandle, size_t size, uint8_t *dst);

        void Close(uint32_t fileHandle);

        void ListDirectory(const nk::String &path);

    private:
        nk::String GetRelativePath(MountPoint *mountPoint, const nk::String &absolutePath);

        MountPoint *FindMountPoint(const nk::String &path);
    };
} // namespace FS

#endif