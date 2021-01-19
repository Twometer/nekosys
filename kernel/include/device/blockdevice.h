#ifndef _BLOCK_DEVICE_H
#define _BLOCK_DEVICE_H

#include <stdint.h>

namespace Device
{

    class IBlockDevice
    {
    protected:
        bool available = false;

    public:
        virtual ~IBlockDevice() = default;

        virtual void ReadBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data) = 0;

        virtual void WriteBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data) = 0;

        bool IsAvailable() { return available; }
    };

} // namespace Device

#endif