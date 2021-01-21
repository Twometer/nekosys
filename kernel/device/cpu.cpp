#include <kernel/device/cpu.h>

void Device::CPU::Halt()
{
    for (;;)
        asm volatile("hlt");
}
