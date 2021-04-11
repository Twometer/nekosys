#ifndef _WINDOW_H
#define _WINDOW_H

#include <nk/string.h>

namespace Gui
{

    class Window
    {
    public:
        nk::String title;
        int width;
        int height;

        Window(const nk::String &title, int width, int height);
    };

};

#endif