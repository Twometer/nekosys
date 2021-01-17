#ifndef _ATA_DISK_H
#define _ATA_DISK_H

#include <kernel/interrupts.h>
#include <device/blockdevice.h>

namespace Disk
{

    class ATADisk : public Device::IBlockDevice, Kernel::InterruptHandler
    {
    private:
        volatile bool interrupted = false;

    public:
        ATADisk(unsigned int disk_num);

        void ReadBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data) override;

        void WriteBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data) override;

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs);
    };

}; // namespace Disk

#endif