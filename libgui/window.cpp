#include <libgui/window.h>

using namespace Gui;

Window::Window(const nk::String &title, int width, int height)
    : title(title), width(width), height(height)
{
}

void Window::Draw()
{
    framebuffer->FillRect(Rectangle(0, 0, width, height), {29, 29, 29});
}

Window::~Window()
{
    delete framebuffer;
}