#include <stdio.h>
#include <stdarg.h>
#include <kernel/device/cpu.h>
#include <kernel/arch/interrupts.h>
#include <kernel/panic.h>
#include <kernel/video/tty.h>

namespace Kernel
{

    void Panic(const char *module, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Interrupts::Disable();

        TTY::SetColor(0x00FF0000);
        printf("\nNK_Kernel Panic:\n%s: ", module);

        TTY::SetColor(0x00AAAAAA);

        vprintf(fmt, args);
        va_end(args);

        Device::CPU::Halt();
    };

} // namespace Kernel