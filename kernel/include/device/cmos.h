#ifndef _CMOS_H
#define _CMOS_H

#include <nk/datetime.h>
#include <stdint.h>

namespace Device
{

    class CMOS
    {
    public:
        static uint8_t Read(uint8_t addr);

        static void Write(uint8_t addr, uint8_t value);

        static nk::datetime GetDate();
    };

} // namespace Device

#endif