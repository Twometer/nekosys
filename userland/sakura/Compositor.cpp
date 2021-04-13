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
        for (size_t x = 0; x < window.width; x++)
        {
            for (size_t y = 0; y < window.height; y++)
            {
                framebuffer->SetPixel(x + window.x, y + window.y, {0, 0, 0});
            }
        }
    }

    framebuffer->SetPixel(mouse->GetPosX(), mouse->GetPosY(), {255, 0, 0});
}