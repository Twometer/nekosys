#include <fs/filesystem.h>

namespace FS
{

    FileSystem::FileSystem(Disk::IBlockDevice *blockDevice, Partition *partition)
        : blockDevice(blockDevice), partition(partition)
    {
    }

} // namespace FS