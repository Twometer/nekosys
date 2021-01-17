#include <disk/atadisk.h>

using namespace Kernel;

namespace Disk
{

    ATADisk::ATADisk(unsigned int disk_num)
    {
        // Interrupts::AddHandler(?, this);
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