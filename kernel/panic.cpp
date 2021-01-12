#include <stdio.h>
#include <stdarg.h>
#include <device/cpu.h>
#include <kernel/interrupts.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

namespace Kernel
{

    void Panic(const char *module, const char *fmt, ...)
    {
        Interrupts::Disable();

        TTY::SetColor(0x0c);
        printf("\nNK_Kernel Panic:\n%s: ", module);

        TTY::SetColor(0x07);
        va_list args;
        va_start(args, fmt);
        printf(fmt, args);
        va_end(args);

        for (;;)
            Device::CPU::Halt();
    };

} // namespace Kernel