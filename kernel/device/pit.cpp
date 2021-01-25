#include <kernel/device/pit.h>
#include <kernel/io.h>
#include <kernel/kdebug.h>

using namespace Kernel;

#define SHIFT_BYTE(value, shift) ((uint8_t)(value & 0xFF) << shift)

namespace Device
{

    void PIT::Configure(int channel, AccessMode accessMode, OperatingMode operatingMode, bool useBcd)
    {
        uint8_t value = SHIFT_BYTE(channel, 6) | SHIFT_BYTE(accessMode, 4) | SHIFT_BYTE(operatingMode, 1) | SHIFT_BYTE(useBcd, 0);
        kdbg("Timer configuring with %x\n", value);
        IO::Out8(0x43, value);
    }

    void PIT::SetSpeed(int channel, int freq)
    {
        uint16_t reloadValue = 1193182 / freq;
        kdbg("Timer channel %d set to frequency %d (%x)\n", channel, freq, reloadValue);
        IO::Out8(0x40 + channel, reloadValue & 0xFF);
        IO::Out8(0x40 + channel, (reloadValue >> 8) & 0xFF);
    }

}; // namespace Device