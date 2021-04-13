#include "Mouse.h"

Mouse::Mouse(uint16_t maxX, uint16_t maxY)
    : maxX(maxX), maxY(maxY)
{
}

void Mouse::Update()
{
    MOUSEPACKET packet{};
    while (mouse_poll(&packet) == 0)
    {
        HandleMousePacket(packet);
    }
}

void Mouse::HandleMousePacket(const MOUSEPACKET &packet)
{
    posX += packet.dx;
    posY -= packet.dy;
    if (posX < 0)
        posX = 0;
    if (posY < 0)
        posY = 0;
    if (posX > maxX - 1)
        posX = maxX - 1;
    if (posY > maxY - 1)
        posY = maxY - 1;
}