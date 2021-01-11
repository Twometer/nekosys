#include <device/cmos.h>
#include <kernel/io.h>

using namespace Device;
using namespace Kernel;

#define PORT_ADDR 0x70
#define PORT_DATA 0x71

#define BCD_TO_BIN(bcd) ((bcd & 0xF0) >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0xf)

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

nk::datetime CMOS::GetDate()
{
    uint8_t format = Read(0x0B);

    nk::datetime datetime = {Read(0x09), Read(0x08), Read(0x07), Read(0x04), Read(0x02), Read(0x00)};
    if (!(format & 4)) {
        datetime.year = BCD_TO_BIN(datetime.year);
        datetime.month = BCD_TO_BIN(datetime.month);
        datetime.day = BCD_TO_BIN(datetime.day);
        datetime.hour = BCD_TO_BIN(datetime.hour);
        datetime.minute = BCD_TO_BIN(datetime.minute);
        datetime.second = BCD_TO_BIN(datetime.second);
    }
    return datetime;
}