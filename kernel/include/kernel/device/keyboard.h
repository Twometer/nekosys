#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <kernel/interrupts.h>

namespace Device
{
    class Keyboard : public Kernel::InterruptHandler
    {
    private:
        char scancode_map[128];

    public:
        void Initialize();

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

    private:
        void NewScancode(unsigned int code, char c);
        char MapChar(unsigned int scancode);
    };
} // namespace Device

#endif