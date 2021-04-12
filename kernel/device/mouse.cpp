#include <kernel/device/mouse.h>
#include <kernel/kdebug.h>
#include <kernel/io.h>
#include <nk/bitfield.h>

using namespace Kernel;
using namespace Device;

#define PS2_PORT_STATUS 0x64
#define PS2_PORT_BUFFER 0x60

Mouse::Mouse()
    : queue(MOUSE_PACKET_NUM)
{
}

void Mouse::Initialize()
{
    kdbg("Initializing mouse...\n");
    Interrupts::AddHandler(12, this);

    // Reset Mouse
    SendCommand(0xFF);
    WaitForReadyToRead();
    auto selfTest = IO::In8(PS2_PORT_BUFFER);
    if (selfTest != 0xaa)
    {
        kdbg("Mouse not detected\n");
        return;
    }

    kdbg("Mouse self-test okay!\n");

    // Read mouse id
    WaitForReadyToRead();
    kdbg("Mouse Id: %x\n", IO::In8(PS2_PORT_BUFFER));

    // enable aux port
    WaitForReadyToWrite();
    IO::Out8(PS2_PORT_STATUS, 0xA8);

    // compaq byte
    WaitForReadyToWrite();
    IO::Out8(PS2_PORT_STATUS, 0x20);
    WaitForReadyToRead();
    auto status = IO::In8(PS2_PORT_BUFFER);
    SET_BIT(status, 1);   // int12 generation
    CLEAR_BIT(status, 5); // mouse clock
    WaitForReadyToWrite();
    IO::Out8(PS2_PORT_STATUS, 0x60);
    WaitForReadyToWrite();
    IO::Out8(PS2_PORT_BUFFER, status);

    // enable automatic packet sending
    SendCommand(0xF6);
    SendCommand(0xF4);
}

uint8_t Mouse::SendCommand(uint8_t command)
{
    WaitForReadyToWrite();
    IO::Out8(PS2_PORT_STATUS, 0xD4);
    WaitForReadyToWrite();
    IO::Out8(PS2_PORT_BUFFER, command);
    WaitForReadyToRead();

    return IO::In8(PS2_PORT_BUFFER);
}

void Mouse::HandleInterrupt(unsigned int, RegisterStates *)
{
    uint8_t status = IO::In8(PS2_PORT_STATUS);

    uint8_t mousePacket[4];
    size_t mousePacketSize = 0;

    while (status & 1)
    {
        uint8_t data = IO::In8(PS2_PORT_BUFFER);
        mousePacket[mousePacketSize++] = data;
        status = IO::In8(PS2_PORT_STATUS);
    }

    if (mousePacketSize != 0)
    {
        int x = mousePacket[1];
        int y = mousePacket[2];
        int w = mousePacket[3] & 0x0f;
        if (w == 15)
            w = -1;

        bool xOverflow = mousePacket[0] & 0x40;
        bool yOverflow = mousePacket[0] & 0x40;
        bool xSign = mousePacket[0] & 0x10;
        bool ySign = mousePacket[0] & 0x20;
        if (x && xSign)
            x -= 255;
        if (y && ySign)
            y -= 255;
        if (xOverflow || yOverflow)
        {
            x = y = 0;
        }
        int buttons = mousePacket[0] & 0x07;

        MousePacket packet = {x, y, w, buttons};
    }
}

void Mouse::WaitForReadyToWrite()
{
    while ((IO::In8(PS2_PORT_STATUS) & 2) != 0)
        ;
}

void Mouse::WaitForReadyToRead()
{
    while ((IO::In8(PS2_PORT_STATUS) & 1) == 0)
        ;
}
