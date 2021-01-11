#include <stdio.h>
#include <device/cpu.h>
#include <kernel/interrupts.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

namespace Kernel
{

    void Panic(const char *message)
    {
        TTY::SetColor(0xcf);
        printf("nekosys: Kernel Panic: \n %s\n", message);
        Interrupts::Disable();
        Device::CPU::Halt();
    };

} // namespace Kernel