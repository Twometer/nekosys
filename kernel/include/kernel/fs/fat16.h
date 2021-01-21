#ifndef _FAT_16
#define _FAT_16

#include <kernel/fs/filesystem.h>

namespace FS
{

    class Fat16 : public FileSystem
    {
    private:
        uint32_t cluster_start;

        uint16_t bytes_per_block;
        uint8_t blocks_per_alloc;
        uint16_t reserved_blocks;
        uint8_t num_fats;
        uint16_t root_dir_entries;
        uint8_t media_descriptor;
        uint16_t blocks_per_fat;
        uint16_t hidden_blocks;
        uint16_t total_sectors;
        uint32_t root_dir_sector;

        // cached structures
        uint8_t *root_directory;
        uint8_t *current_cluster;
        uint8_t *fat_cache;

    public:
        Fat16(Disk::IBlockDevice *blockDevice, Partition *partition);

        bool Exists(const nk::String &path) override;

        DirEntry GetFileMeta(const nk::String &path) override;

        void Read(const DirEntry &entry, size_t size, uint8_t *dst) override;

        void ListDirectory(const nk::String &path) override;

    private:
        DirEntry ParseDirEntry(uint8_t *data, bool &eof);

        void LoadCluster(uint32_t cluster);
    };

} // namespace FS

#endif