#include <stdio.h>
#include <device/cpu.h>
#include <kernel/interrupts.h>
#include <kernel/panic.h>

namespace Kernel
{

    void Panic(const char *message)
    {
        printf("nekosys: Kernel Panic: \n %s\n", message);
        Interrupts::Disable();
        Device::CPU::Halt();
    };

} // namespace Kernel