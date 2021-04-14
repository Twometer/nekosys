#ifndef _COMPOSITOR_H
#define _COMPOSITOR_H

#include <nk/vector.h>
#include "Mouse.h"
#include "WindowInfo.h"
#include "DirtyRegionManager.h"

class Compositor
{
private:
    /* The windows are represented as a doubly linked list, so that moving windows to the front is a O(1) operation */
    WindowInfo *windows_head = nullptr;
    WindowInfo *windows_tail = nullptr;

    Bitmap *framebuffer;
    Bitmap *wallpaper;
    Bitmap cursor = Bitmap("/res/cursors/default.png");
    Mouse *mouse;

    DirtyRegionManager *dirtyManager;

public:
    Compositor(Bitmap *framebuffer, Bitmap *wallpaper);

    ~Compositor();

    void AddWindow(WindowInfo *window);

    void BringToFront(WindowInfo *window);

    Rectangle RenderFrame();
};

#endif