#include "Compositor.h"

#define MOUSE_UPDATE_SIZE 20

Compositor::Compositor(Bitmap *framebuffer, Bitmap *wallpaper)
    : framebuffer(framebuffer),
      wallpaper(wallpaper),
      mouse(new Mouse(framebuffer->width, framebuffer->height))
{
    dirtyManager = new DirtyRegionManager();
    dirtyManager->MarkDirty(Rectangle(0, 0, framebuffer->width, framebuffer->height));
}

Compositor::~Compositor()
{
    delete mouse;
}

void Compositor::AddWindow(WindowInfo *window)
{
    if (windows_head == nullptr && windows_tail == nullptr)
    {
        windows_head = window;
        windows_tail = window;
    }
    else
    {
        window->prev = windows_tail;
        windows_tail->next = window;
        windows_tail = window;
    }
    dirtyManager->MarkDirty(window->clientRectangle());
}

void Compositor::BringToFront(WindowInfo *window)
{
    // If window is not already at the front
    if (window->prev != nullptr)
    {
        // Cut window out of the middle
        window->prev->next = window->next;
        window->next->prev = window->prev;

        // And put at start of list
        window->next = windows_head;
        windows_head->prev = window;
        windows_head = window;
    }
}

Rectangle Compositor::RenderFrame()
{
    Point oldMousePos = {mouse->GetPosX(), mouse->GetPosY()};
    mouse->Update();
    if (oldMousePos.x != mouse->GetPosX() || oldMousePos.y != mouse->GetPosY())
    {
        dirtyManager->MarkDirty(Rectangle(oldMousePos.x, oldMousePos.y, MOUSE_UPDATE_SIZE, MOUSE_UPDATE_SIZE));
        dirtyManager->MarkDirty(Rectangle(mouse->GetPosX(), mouse->GetPosY(), MOUSE_UPDATE_SIZE, MOUSE_UPDATE_SIZE));
    }

    if (!dirtyManager->AnyDirty())
        return Rectangle(0, 0, 0, 0);

    auto rect = dirtyManager->GetRectangle();
    // printf("Redrawing %d,%d %dx%d\n", rect.x0, rect.y0, rect.size().width, rect.size().height);

    framebuffer->Blit(*wallpaper, rect.position(), rect);

    WindowInfo *window = windows_head;
    while (window != nullptr)
    {
        auto clientRect = window->clientRectangle();
        if (rect.Intersects(clientRect))
        {
            auto dirtyRect = clientRect.Intersection(rect);
            framebuffer->Blit(*window->bitmap, {dirtyRect.x0 - clientRect.x0, dirtyRect.y0 - clientRect.y0}, dirtyRect);
            framebuffer->DrawText(window->title, font, {window->x, window->y}, {125, 125, 125, 255});
        }

        window = window->next;
    }

    framebuffer->DrawText("sakura_compositor 0.01", font, {0, 0}, {0, 0, 255, 255});

    DrawCursor();
    dirtyManager->Reset();
    return rect;
}

void Compositor::DrawCursor()
{
    framebuffer->DrawBitmap(cursor, Rectangle(mouse->GetPosX(), mouse->GetPosY(), cursor.width, cursor.height));
}