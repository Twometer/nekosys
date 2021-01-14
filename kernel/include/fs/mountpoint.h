#ifndef _MOUNT_POINT_H
#define _MOUNT_POINT_H

#include <fs/filesystem.h>

namespace FS
{
    struct MountPoint
    {
        const char *path;
        FileSystem *fs;
    };
} // namespace FS

#endif