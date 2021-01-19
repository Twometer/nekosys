#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H

#include <stdint.h>
#include <fs/filesystem.h>
#include <fs/direntry.h>

namespace FS
{

    struct FileHandle
    {
        uint32_t id;
        FileSystem *fs;
        DirEntry *entry;
        uint32_t offset;
    };

} // namespace FS

#endif