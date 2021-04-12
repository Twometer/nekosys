#include <kernel/device/mouse.h>
#include <kernel/kdebug.h>
#include <kernel/io.h>
#include <nk/bitfield.h>

using namespace Kernel;
using namespace Device;

#define PS2_PORT_STATUS 0x64
#define PS2_PORT_BUFFER 0x60

#define PS2_MSG_GET_STATUS 0x20
#define PS2_MSG_SET_STATUS 0x60

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

    // load defaults
    auto defaults = SendCommand(0xF6);
    kdbg("Defaults: %x\n", defaults);

    // enable packet streaming
    auto enable = SendCommand(0xF4);
    kdbg("Enabled: %x\n", enable);
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
    kdbg("Mouse int!\n");
}

void Mouse::WaitForReadyToWrite()
{
    while (IO::In8(PS2_PORT_STATUS) & 2 != 0)
        ;
}

void Mouse::WaitForReadyToRead()
{
    while (IO::In8(PS2_PORT_STATUS) & 1 == 0)
        ;
}
