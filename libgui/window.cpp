#include <libgui/window.h>

using namespace Gui;

Window::Window(const nk::String &title, int width, int height)
    : title(title), width(width), height(height)
{
}