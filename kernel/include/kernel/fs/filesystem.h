#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include <kernel/disk/blockdevice.h>
#include <kernel/fs/partition.h>
#include <kernel/fs/direntry.h>
#include <nk/string.h>
#include <nk/vector.h>

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

        virtual void Read(const DirEntry &entry, size_t offset, size_t size, uint8_t *dst) = 0;

        virtual nk::Vector<DirEntry> ListDirectory(const nk::String &path) = 0;
    };

} // namespace FS

#endif