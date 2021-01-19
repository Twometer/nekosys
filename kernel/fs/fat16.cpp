#include <stdio.h>
#include <fs/fat16.h>
#include <nk/buffer.h>

#define FAT16_DEBUG 0

namespace FS
{

    Fat16::Fat16(Disk::IBlockDevice *blockDevice, Partition *partition) : FileSystem(blockDevice, partition)
    {
        auto baseSector = new uint8_t[512];
        blockDevice->ReadBlock(partition->startSector, 1, baseSector);

        nk::Buffer buf(baseSector, 512);
        bytes_per_blocK = buf.Read<uint16_t>(0x0b);
        blocks_per_alloc = buf.Read<uint8_t>(0x0d);
        reserved_blocks = buf.Read<uint16_t>(0x0e);
        num_fats = buf.Read<uint8_t>(0x10);
        root_dir_entries = buf.Read<uint16_t>(0x11);
        media_descriptor = buf.Read<uint8_t>(0x15);
        blocks_per_fat = buf.Read<uint16_t>(0x16);
        hidden_blocks = buf.Read<uint16_t>(0x1c);
        total_sectors = buf.Read<uint16_t>(0x20);
        delete baseSector;

        root_dir_sector = partition->startSector + reserved_blocks + blocks_per_fat * num_fats;

        auto rootdirSize = 32 * root_dir_entries;
        root_directory = new uint8_t[rootdirSize];
        blockDevice->ReadBlock(root_dir_sector, rootdirSize / 512, root_directory);

#if FAT16_DEBUG
        printf("fat_16: Root dir at %d with %d entries\n", (int)root_dir_sector, (int)root_dir_entries);
#endif
    }

    bool Fat16::Exists(const nk::String &path)
    {
        return false;
    }

    DirEntry *Fat16::GetFileMeta(const nk::String &path)
    {
        return nullptr;
    }

    void Fat16::ListDirectory(const nk::String &path)
    {
        bool eof = false;
        for (size_t i = 0; i < 512; i += 32)
        {
            auto entry = ParseDirEntry(root_directory + i, eof);
            if (eof)
                break;

            if (entry.type == DirEntryType::File)
            {
                printf(" [f] %s\n", entry.name.CStr());
            }
            else if (entry.type == DirEntryType::Folder)
            {
                printf(" [d] %s\n", entry.name.CStr());
            }
        }
    }

    DirEntry Fat16::ParseDirEntry(uint8_t *data, bool &eof)
    {
        DirEntry result{};
        nk::Buffer dirbuf(data, 32);
        eof = false;

        char *filename = (char *)dirbuf.At(0);
        if (filename[0] == 0x00) // this is empty
        {
            eof = true;
            return result;
        }

        uint8_t attr = dirbuf.Read<uint8_t>(0x0B);
        uint16_t perms = dirbuf.Read<uint16_t>(0x14);
        uint16_t lastModifiedTime = dirbuf.Read<uint16_t>(0x16);
        uint16_t lastModifiedDate = dirbuf.Read<uint16_t>(0x18);
        uint16_t cluster = dirbuf.Read<uint16_t>(0x1A);
        uint32_t filesize = dirbuf.Read<uint32_t>(0x1C);

        if (attr & 0x02)
            result.type = DirEntryType::Hidden;
        else if (attr & 0x08)
            result.type = DirEntryType::VolumeLabel;
        else if (attr & 0x10)
            result.type = DirEntryType::Folder;
        else if (attr & 0x04)
            result.type = DirEntryType::System;
        else
            result.type = DirEntryType::File;

        result.name = filename;
        result.location = cluster;
        result.size = filesize;

        return result;
    }

}; // namespace FS