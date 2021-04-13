#ifndef _WINDOW_INFO_H
#define _WINDOW_INFO_H

#include <nk/string.h>
#include <gfx/bitmap.h>
#include <gfx/rectangle.h>

struct WindowInfo
{
    nk::String title;
    int x, y, width, height;
    int shbufId;
    Bitmap *bitmap;

    Rectangle clientRectangle()
    {
        return Rectangle(x, y, width, height);
    }
};

#endif