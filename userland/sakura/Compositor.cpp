#include "Compositor.h"

#define MOUSE_UPDATE_SIZE 24

Compositor::Compositor(Bitmap *framebuffer, Bitmap *wallpaper)
    : framebuffer(framebuffer),
      wallpaper(wallpaper),
      windows(new nk::Vector<WindowInfo>()),
      mouse(new Mouse(framebuffer->width, framebuffer->height))
{
    dirtyManager = new DirtyRegionManager();
    dirtyManager->MarkDirty(Rectangle(0, 0, framebuffer->width, framebuffer->height));
}

Compositor::~Compositor()
{
    delete windows;
    delete mouse;
}

void Compositor::AddWindow(WindowInfo window)
{
    windows->Add(window);
}

void Compositor::RenderFrame()
{
    Point oldMousePos = {mouse->GetPosX(), mouse->GetPosY()};
    mouse->Update();
    if (oldMousePos.x != mouse->GetPosX() || oldMousePos.y != mouse->GetPosY())
    {
        dirtyManager->MarkDirty(Rectangle(oldMousePos.x, oldMousePos.y, MOUSE_UPDATE_SIZE, MOUSE_UPDATE_SIZE));
        dirtyManager->MarkDirty(Rectangle(mouse->GetPosX(), mouse->GetPosY(), MOUSE_UPDATE_SIZE, MOUSE_UPDATE_SIZE));
    }

    if (!dirtyManager->AnyDirty())
        return;

    auto rect = dirtyManager->GetRectangle();

    // printf("redrawing %d,%d %dx%d\n", rect.position.x, rect.position.y, rect.size.width, rect.size.height);

    framebuffer->Blit(*wallpaper, {rect.position.x, rect.position.y}, rect);

    for (size_t i = 0; i < windows->Size(); i++)
    {
        auto window = windows->At(i);
        auto renderRect = Rectangle(window.x, window.y, window.width, window.height);

        if (renderRect.Intersects(rect))
        {
            framebuffer->Blit(*window.bitmap, {0, 0}, renderRect);
        }
    }

    framebuffer->DrawBitmap(cursor, Rectangle(mouse->GetPosX(), mouse->GetPosY(), cursor.width, cursor.height));

    dirtyManager->Reset();
}