#include "Compositor.h"

Compositor::Compositor(Bitmap *framebuffer, Bitmap *wallpaper)
    : framebuffer(framebuffer),
      wallpaper(wallpaper),
      windows(new nk::Vector<WindowInfo>()),
      mouse(new Mouse(framebuffer->width, framebuffer->height))
{
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
    mouse->Update();
    framebuffer->Blit(*wallpaper, Rectangle(0, 0, framebuffer->width, framebuffer->height));

    for (size_t i = 0; i < windows->Size(); i++)
    {
        auto window = windows->At(i);
        framebuffer->Blit(*window.bitmap, Rectangle(window.x, window.y, window.width, window.height));
    }

    framebuffer->SetPixel(mouse->GetPosX(), mouse->GetPosY(), {255, 0, 0});
}