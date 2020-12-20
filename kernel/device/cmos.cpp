#include <device/cmos.h>
#include <kernel/io.h>

using namespace Device;
using namespace Kernel;

#define PORT_ADDR 0x70
#define PORT_DATA 0x71

uint8_t CMOS::Read(uint8_t addr)
{
    IO::Out8(PORT_ADDR, addr);
    return IO::In8(PORT_DATA);
}

void CMOS::Write(uint8_t addr, uint8_t value)
{
    IO::Out8(PORT_ADDR, addr);
    IO::Out8(PORT_DATA, value);
}