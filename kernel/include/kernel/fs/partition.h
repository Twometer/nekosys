#ifndef _PARTITION_H
#define _PARTITION_H

namespace FS
{

    struct Partition
    {
        bool bootable;
        uint8_t partitionType;
        uint32_t startSector;
        uint32_t numSectors;

        Partition(bool bootable, uint8_t partitionType, uint32_t startSector, uint32_t numSectors)
            : bootable(bootable), partitionType(partitionType), startSector(startSector), numSectors(numSectors){};
            
    };

} // namespace FS

#endif