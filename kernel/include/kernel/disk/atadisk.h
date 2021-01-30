#ifndef _ATA_DISK_H
#define _ATA_DISK_H

#include <kernel/arch/interrupts.h>
#include <kernel/disk/blockdevice.h>

namespace Disk
{

    class ATADisk : public Disk::IBlockDevice, Kernel::InterruptHandler
    {
    private:
        int max_sectors = 0;

        volatile bool interrupted = false;

    public:
        ATADisk(unsigned int disk_num);

        void ReadBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data) override;

        void WriteBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data) override;

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs);

    private:
        void WaitForInterrupt();

        void ReadRaw(uint16_t *dst, size_t num_words);
    };

}; // namespace Disk

#endif