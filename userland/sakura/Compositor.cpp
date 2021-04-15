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
    dirtyManager->MarkDirty(window->rectangle());
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

void Compositor::HandleControls()
{
    WindowInfo *window = windows_head;

    while (window != nullptr)
    {
        if (mouse->IsButtonDown(MouseButton::Left) && window->dragRectangle().Intersects(mouse->position()) && !window->dragging)
        {
            window->dragging = true;
            window->draggingOffset = {mouse->GetPosX() - window->x, mouse->GetPosY() - window->y};
        }

        if (window->dragging && !mouse->IsButtonDown(MouseButton::Left))
        {
            window->dragging = false;
        }

        if (window->dragging)
        {
            dirtyManager->MarkDirty(window->rectangle());
            window->x = mouse->position().x - window->draggingOffset.x;
            window->y = mouse->position().y - window->draggingOffset.y;
            dirtyManager->MarkDirty(window->rectangle());
        }

        window = window->next;
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

    HandleControls();

    if (!dirtyManager->AnyDirty())
        return Rectangle(0, 0, 0, 0);

    auto dirtyRect = dirtyManager->GetRectangle();

    // printf("Redrawing %d,%d %dx%d\n", rect.x0, rect.y0, rect.size().width, rect.size().height);
    framebuffer->SetMask(dirtyRect);
    framebuffer->Blit(*wallpaper, {0, 0});

    WindowInfo *window = windows_head;
    while (window != nullptr)
    {
        // Window contents
        framebuffer->Blit(*window->bitmap, window->clientRectangle().position());

        // Window title bar
        framebuffer->FillGradient(Rectangle(window->x, window->y, window->width, WINDOW_TITLE_HEIGHT), {9, 37, 104, 255}, {165, 198, 247, 255});
        framebuffer->DrawText(window->title, font, {window->x + 8, window->y + 3}, {204, 204, 204, 255});
        framebuffer->DrawText("X", font, {window->x + window->width - 16, window->y + 3}, {0, 0, 0, 255});

        window = window->next;
    }

    framebuffer->DrawText("sakura_compositor 0.01", font, {0, 0}, {0, 0, 255, 255});

    DrawCursor();
    dirtyManager->Reset();
    return dirtyRect;
}

void Compositor::DrawCursor()
{
    framebuffer->Blit(cursor, {mouse->GetPosX(), mouse->GetPosY()}, BlendMode::Alpha);
}