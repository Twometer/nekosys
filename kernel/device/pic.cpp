#include <kernel/device/pic.h>
#include <kernel/kdebug.h>
#include <kernel/io.h>

using namespace Device;
using namespace Kernel;

void PIC::Remap()
{
    unsigned char m1, m2;
    m1 = IO::In8(0x21);
    m2 = IO::In8(0xA1);

    IO::Out8(0x20, 0x11);
    IO::Out8(0xA0, 0x11);
    IO::Out8(0x21, 0x20);
    IO::Out8(0xA1, 40);
    IO::Out8(0x21, 0x04);
    IO::Out8(0xA1, 0x02);
    IO::Out8(0x21, 0x01);
    IO::Out8(0xA1, 0x01);
    IO::Out8(0x21, 0x0);
    IO::Out8(0xA1, 0x0);

    IO::Out8(0x21, m1);
    IO::Out8(0xA1, m2);

    kdbg("PIC Mask: %x %x\n", m1, m2);
}