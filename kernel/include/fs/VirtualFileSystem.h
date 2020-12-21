#ifndef _VIRTUAL_FILE_SYSTEM_H
#define _VIRTUAL_FILE_SYSTEM_H

#include <stdint.h>
#include <nk/vector.h>
#include "MountPoint.h"
#include "File.h"

namespace FS
{
    class VirtualFileSystem
    {
    private:
        nk::Vector<MountPoint *> mounts;

    public:
        void Mount(const char *path, FileSystem *fs);

        void Unmount(const char *path);

        bool Exists(const char *path);

        File *GetFile(const char *path);

        uint8_t *ReadFile(File *file);
    };
} // namespace FS

#endif