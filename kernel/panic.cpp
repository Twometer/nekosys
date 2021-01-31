#include <stdio.h>
#include <stdarg.h>
#include <kernel/device/cpu.h>
#include <kernel/arch/interrupts.h>
#include <kernel/panic.h>
#include <kernel/video/videomanager.h>

using namespace Video;

namespace Kernel
{

    void Panic(const char *module, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Interrupts::Disable();

        VideoManager::GetInstance()->GetTTY()->SetColor(TerminalColor::LightRed);
        printf("\nNK_Kernel Panic:\n%s: ", module);

        VideoManager::GetInstance()->GetTTY()->SetColor(TerminalColor::LightGray);
        vprintf(fmt, args);
        va_end(args);

        Device::CPU::Halt();
    };

} // namespace Kernel