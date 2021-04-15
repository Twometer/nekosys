#include <libgui/window.h>

using namespace Gui;

Window::Window(const nk::String &title, int width, int height)
    : title(title), width(width), height(height)
{
}

void Window::Draw()
{
    framebuffer->ClearMask();
    framebuffer->FillRect(Rectangle(0, 0, width, height), {30, 30, 30});

    Font font("/res/fonts/opsans.fnt");
    framebuffer->DrawText("Hello from the other side!", font, {5, 5}, {255, 255, 255, 255});
}

Window::~Window()
{
    delete framebuffer;
}