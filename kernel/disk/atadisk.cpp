#include <kernel/io.h>
#include <disk/atadisk.h>

using namespace Kernel;

#define ATA_PRIMARY_PORT 0x1F0
#define ATA_PRIMARY_DEVICE_CONTROL_REG 0x3F6
#define ATA_PRIMARY_ALTERNATE_STATUS_REG 0x376
#define ATA_PRIMARY_IRQ 14

#define ATA_SECONDARY_PORT 0x170
#define ATA_SECONDARY_DEVICE_CONTROL_REG 0x3E6
#define ATA_SECONDARY_ALTERNATE_STATUS_REG 0x366
#define ATA_SECONDARY_IRQ 15

namespace Disk
{

    ATADisk::ATADisk(unsigned int disk_num)
    {
        Interrupts::AddHandler(ATA_PRIMARY_IRQ, this);
    }

    void ATADisk::ReadBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data)
    {
    }

    void ATADisk::WriteBlock(uint64_t block_idx, uint64_t block_num, uint8_t *data)
    {
    }

    void ATADisk::HandleInterrupt(unsigned int interrupt, RegisterStates *regs)
    {
        interrupted = true;
    }

}; // namespace Disk