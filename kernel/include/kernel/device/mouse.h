#ifndef _MOUSE_H
#define _MOUSE_H

#include <kernel/arch/interrupts.h>

namespace Device
{
    class Mouse : public Kernel::InterruptHandler
    {
    public:
        void Initialize();

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

    private:
        void WaitForReadyToWrite();

        void WaitForReadyToRead();

        uint8_t SendCommand(uint8_t command);
    };
};

#endif