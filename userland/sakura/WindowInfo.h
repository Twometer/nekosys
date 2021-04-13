#ifndef _WINDOW_INFO_H
#define _WINDOW_INFO_H

#include <nk/string.h>
#include <gfx/bitmap.h>

struct WindowInfo
{
    nk::String title;
    int x, y, width, height;
    int shbufId;
    Bitmap *bitmap;
};

#endif