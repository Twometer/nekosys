#include <stdio.h>
#include <fs/fat16.h>

namespace FS
{

    Fat16::Fat16(Disk::IBlockDevice *blockDevice, Partition *partition) : FileSystem(blockDevice, partition)
    {
    }

    bool Fat16::Exists(const nk::String &path)
    {
        return false;
    }

    File *Fat16::GetFile(const nk::String &path)
    {
        return nullptr;
    }

    uint8_t *Fat16::ReadFile(File *file)
    {
        return nullptr;
    }

    void Fat16::ListDirectory(const nk::String &path)
    {
        printf("fat_16: Listing directory %s\n", path.CStr());
    }

}; // namespace FS