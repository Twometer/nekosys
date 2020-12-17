#include <device/cpu.h>

void Device::CPU::Halt()
{
    for (;;)
        asm("hlt");
}
