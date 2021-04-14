#ifndef _COMPOSITOR_H
#define _COMPOSITOR_H

#include <nk/vector.h>
#include "Mouse.h"
#include "WindowInfo.h"
#include "DirtyRegionManager.h"

class Compositor
{
private:
    nk::Vector<WindowInfo> *windows;

    Bitmap *framebuffer;
    Bitmap *renderbuffer;
    Bitmap *wallpaper;
    Bitmap cursor = Bitmap("/res/cursors/default.png");
    Mouse *mouse;

    DirtyRegionManager *dirtyManager;

public:
    Compositor(Bitmap *framebuffer, Bitmap *wallpaper);

    ~Compositor();

    void AddWindow(WindowInfo window);

    Rectangle RenderFrame();
};

#endif