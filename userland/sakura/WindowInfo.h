#ifndef _WINDOW_INFO_H
#define _WINDOW_INFO_H

#include <nk/string.h>
#include <gfx/bitmap.h>
#include <gfx/rectangle.h>

#define WINDOW_TITLE_HEIGHT 24

struct WindowInfo
{
    nk::String title;
    int x, y, width, height;
    int shbufId;
    Bitmap *bitmap;

    WindowInfo *prev = nullptr;
    WindowInfo *next = nullptr;

    bool dragging = false;
    Point draggingOffset;

    WindowInfo(const nk::String &title, int x, int y, int width, int height, int shbufId, Bitmap *bitmap)
        : title(title), x(x), y(y), width(width), height(height), shbufId(shbufId), bitmap(bitmap)
    {
    }

    Rectangle clientRectangle()
    {
        return Rectangle(x, y + WINDOW_TITLE_HEIGHT, width, height);
    }

    Rectangle rectangle()
    {
        return Rectangle(x, y, width, height + WINDOW_TITLE_HEIGHT);
    }

    Rectangle dragRectangle()
    {
        return Rectangle(x, y, width, WINDOW_TITLE_HEIGHT);
    }
};

#endif