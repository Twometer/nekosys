#ifndef _WINDOW_H
#define _WINDOW_H

#include <stdint.h>
#include <nk/string.h>
#include <gfx/bitmap.h>

namespace Gui
{

    class Window
    {
    public:
        nk::String title;
        int x = 32;
        int y = 32;
        int width;
        int height;

        int shbufId;
        uint8_t *buffer;
        Bitmap *framebuffer;

        Window(const nk::String &title, int width, int height);

        ~Window();

        void Draw();
    };

};

#endif