#include "Compositor.h"

Compositor::Compositor(Bitmap *framebuffer, Bitmap *wallpaper)
    : framebuffer(framebuffer),
      wallpaper(wallpaper),
      windows(new nk::Vector<WindowInfo>()),
      mouse(new Mouse(framebuffer->width, framebuffer->height))
{
    renderbuffer = new Bitmap(framebuffer->width, framebuffer->height, PixelFormat::Rgb24);
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
    renderbuffer->Blit(*wallpaper, Rectangle(0, 0, framebuffer->width, framebuffer->height));

    for (size_t i = 0; i < windows->Size(); i++)
    {
        auto window = windows->At(i);
        renderbuffer->Blit(*window.bitmap, Rectangle(window.x, window.y, window.width, window.height));
    }

    renderbuffer->DrawBitmap(cursor, Rectangle(mouse->GetPosX(), mouse->GetPosY(), cursor.width, cursor.height));

    framebuffer->Blit(*renderbuffer, Rectangle(0, 0, framebuffer->width, framebuffer->height));
}