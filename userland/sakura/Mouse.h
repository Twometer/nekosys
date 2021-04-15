#ifndef _MOUSE_H
#define _MOUSE_H

#include <nekosys.h>
#include <libgui/mousebutton.h>
#include <gfx/point.h>

class Mouse
{
private:
    int posX;
    int posY;

    uint16_t maxX;
    uint16_t maxY;

    uint16_t buttons;

public:
    Mouse(uint16_t maxX, uint16_t maxY);

    void Update();

    inline uint16_t GetPosX() const { return posX; };

    inline uint16_t GetPosY() const { return posY; };

    inline uint16_t GetButtons() const { return buttons; };

    Point position() { return {posX, posY}; }

    bool IsButtonDown(MouseButton button) const { return (buttons & (uint16_t)buttons) != 0; }

private:
    void HandleMousePacket(const MOUSEPACKET &packet);
};

#endif