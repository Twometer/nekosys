#include <fs/mbr.h>

#define MBR_BASE 0x1BE

namespace FS
{

    const nk::Vector<Partition *> MBR::Parse(Device::IBlockDevice *device)
    {
        uint8_t *mbr = new uint8_t[512];
        device->ReadBlock(0, 1, mbr);

        nk::Vector<Partition *> partitions;
        for (size_t i = 0x1BE; i < 510; i += 16)
        {
            bool bootable = mbr[i] == 0x80;
            uint8_t partitionType = mbr[i + 0x04];
            uint32_t startSector = *(uint32_t *)&mbr[i + 0x08];
            uint32_t numSectors = *(uint32_t *)&mbr[i + 0x0C];

            if (numSectors != 0)
                partitions.Add(new Partition(bootable, partitionType, startSector, numSectors));
        }

        delete mbr;
        return partitions;
    }

}; // namespace FS