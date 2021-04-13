#ifndef _MOUSE_H
#define _MOUSE_H

#include <nekosys.h>

class Mouse
{
private:
    int posX;
    int posY;

    uint16_t maxX;
    uint16_t maxY;

public:
    Mouse(uint16_t maxX, uint16_t maxY);

    void Update();

    inline uint16_t GetPosX() const { return posX; };

    inline uint16_t GetPosY() const { return posY; };

private:
    void HandleMousePacket(const MOUSEPACKET &packet);
};

#endif