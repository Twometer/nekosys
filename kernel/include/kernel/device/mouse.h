#ifndef _MOUSE_H
#define _MOUSE_H

#include <nk/circularqueue.h>
#include <kernel/arch/interrupts.h>

// Number of mouse packets the mouse saves before it overwrites the oldest one.
#define MOUSE_PACKET_NUM 256

#define MOUSEBTN_LEFT 1
#define MOUSEBTN_RIGHT 2
#define MOUSEBTN_WHEEL 4

namespace Device
{
    struct MousePacket
    {
        int dx;
        int dy;
        int dwheel;
        int buttons;
    };

    class Mouse : public Kernel::InterruptHandler
    {
    public:
        int posX;
        int posY;
        int wheelPosition;
        int buttons;

        nk::CircularQueue<MousePacket> queue;

        Mouse();

        void Initialize();

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

    private:
        void WaitForReadyToWrite();

        void WaitForReadyToRead();

        uint8_t SendCommand(uint8_t command);
    };
};

#endif