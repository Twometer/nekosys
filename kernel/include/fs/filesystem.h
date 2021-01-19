#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include <disk/blockdevice.h>
#include <fs/partition.h>
#include <fs/file.h>
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

        virtual File *GetFile(const nk::String &path) = 0;

        virtual uint8_t *ReadFile(File *file) = 0;

        virtual void ListDirectory(const nk::String &path) = 0;
    };

} // namespace FS

#endif