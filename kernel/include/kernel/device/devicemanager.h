#ifndef _DEVICEMANAGER_H
#define _DEVICEMANAGER_H

#include "cpu.h"
#include "keyboard.h"
#include "pic.h"
#include "mouse.h"
#include "cmos.h"

namespace Device
{

    class DeviceManager
    {
    public:
        static Keyboard *keyboard;

        static Mouse *mouse;

        static PIC *pic;

        static void Initialize();
    };

} // namespace Device

#endif