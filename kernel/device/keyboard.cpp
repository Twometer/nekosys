#include <kernel/tty.h>
#include <kernel/interrupts.h>
#include <device/keyboard.h>

using namespace Device;

char Keyboard::scancode_map[128];

void Keyboard::Initialize()
{
    for (int i = 0; i < 128; i++)
        scancode_map[i] = '\0';

    NewScancode(2, '1');
    NewScancode(3, '2');
    NewScancode(4, '3');
    NewScancode(5, '4');
    NewScancode(6, '5');
    NewScancode(7, '6');
    NewScancode(8, '7');
    NewScancode(9, '8');
    NewScancode(10, '9');
    NewScancode(11, '0');
    //NewScancode(12, 'ß');
    //NewScancode(13, '´');
    NewScancode(14, '\b');
    NewScancode(15, '\t');
    NewScancode(16, 'q');
    NewScancode(17, 'w');
    NewScancode(18, 'e');
    NewScancode(19, 'r');
    NewScancode(20, 't');
    NewScancode(21, 'z');
    NewScancode(22, 'u');
    NewScancode(23, 'i');
    NewScancode(24, 'o');
    NewScancode(25, 'p');
    //NewScancode(26, 'ü');
    NewScancode(27, '*');
    NewScancode(28, '\n');
    NewScancode(30, 'a');
    NewScancode(31, 's');
    NewScancode(32, 'd');
    NewScancode(33, 'f');
    NewScancode(34, 'g');
    NewScancode(35, 'h');
    NewScancode(36, 'j');
    NewScancode(37, 'k');
    NewScancode(38, 'l');
    //NewScancode(39, 'ö');
    //NewScancode(40, 'ä');
    NewScancode(43, '#');
    NewScancode(86, '<');
    NewScancode(44, 'y');
    NewScancode(45, 'x');
    NewScancode(46, 'c');
    NewScancode(47, 'v');
    NewScancode(48, 'b');
    NewScancode(49, 'n');
    NewScancode(50, 'm');
    NewScancode(51, ',');
    NewScancode(52, '.');
    NewScancode(53, '-');
    NewScancode(57, ' ');

}

void Keyboard::HandleInterrupt(unsigned int scancode)
{
    if (scancode > 128)
        return;

    char chr = MapChar(scancode);
    if (chr != 0)
        Kernel::TTY::Write(&chr, 1);
}

void Keyboard::NewScancode(unsigned int scancode, char c)
{
    scancode_map[scancode] = c;
}

char Keyboard::MapChar(unsigned int scancode)
{
    if (scancode < 128)
        return scancode_map[scancode];
    else
        return '\0';
}