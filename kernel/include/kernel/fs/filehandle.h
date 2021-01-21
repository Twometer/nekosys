#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H

#include <stdint.h>
#include <kernel/fs/filesystem.h>
#include <kernel/fs/direntry.h>

namespace FS
{

    struct FileHandle
    {
        uint32_t id;
        FileSystem *fs;
        DirEntry entry;
        uint32_t offset;

        FileHandle(uint32_t id, FileSystem *fs, DirEntry entry)
            : id(id), fs(fs), entry(entry), offset(0) {}
            
    };

} // namespace FS

#endif