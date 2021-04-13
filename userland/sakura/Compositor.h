#ifndef _COMPOSITOR_H
#define _COMPOSITOR_H

#include <nk/vector.h>
#include "Mouse.h"
#include "WindowInfo.h"

class Compositor
{
private:
    nk::Vector<WindowInfo> *windows;

    Bitmap *framebuffer;
    Bitmap *wallpaper;
    Bitmap cursor = Bitmap("/usr/cursors/default.png");
    Mouse *mouse;

public:
    Compositor(Bitmap *framebuffer, Bitmap *wallpaper);

    ~Compositor();

    void AddWindow(WindowInfo window);

    void RenderFrame();
};

#endif