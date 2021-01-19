#ifndef _FAT_16
#define _FAT_16

#include <fs/filesystem.h>

namespace FS
{

    class Fat16 : public FileSystem
    {
    public:
        Fat16(Disk::IBlockDevice *blockDevice, Partition *partition);

        bool Exists(const nk::String &path) override;

        File *GetFile(const nk::String &path) override;

        uint8_t *ReadFile(File *file) override;

        void ListDirectory(const nk::String &path) override;
    };

} // namespace FS

#endif