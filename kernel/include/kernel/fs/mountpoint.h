#ifndef _MOUNT_POINT_H
#define _MOUNT_POINT_H

#include <nk/string.h>
#include <kernel/fs/filesystem.h>

namespace FS
{
    struct MountPoint
    {
        nk::String path;
        FileSystem *fs;

        MountPoint(const nk::String &path, FileSystem *fs)
            : path(path), fs(fs){};
    };
} // namespace FS

#endif