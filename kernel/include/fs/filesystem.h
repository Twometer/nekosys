#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include <disk/blockdevice.h>
#include <fs/partition.h>
#include <fs/direntry.h>
#include <nk/string.h>

namespace FS
{

    class FileSystem
    {
    protected:
        Disk::IBlockDevice *blockDevice;
        Partition *partition;

    public:
        FileSystem(Disk::IBlockDevice *blockDevice, Partition *partition);

        virtual ~FileSystem() = default;

        virtual bool Exists(const nk::String &path) = 0;

        virtual DirEntry GetFileMeta(const nk::String &path) = 0;

        virtual void Read(const DirEntry &entry, size_t size, uint8_t *dst) = 0;

        virtual void ListDirectory(const nk::String &path) = 0;
    };

} // namespace FS

#endif