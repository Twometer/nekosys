#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stddef.h>
#include <nk/vector.h>
#include <kernel/arch/interrupts.h>

namespace Device
{
    class Keyboard : public Kernel::InterruptHandler
    {
    private:
        char scancode_map[128];

        nk::Vector<char> buf;

    public:
        void Initialize();

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

        const nk::Vector<char> &GetBuffer() { return buf; };

        size_t ReadUntil(char *dst, size_t maxSize, char delim);

    private:
        void NewScancode(unsigned int code, char c);
        char MapChar(unsigned int scancode);
    };
} // namespace Device

#endif