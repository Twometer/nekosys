#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "point.h"
#include "size.h"

struct Rectangle
{
    Point position;
    Size size;

    Rectangle(Point position, Size size)
        : position(position), size(size)
    {
    }

    Rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
        : position({x, y}), size({width, height})
    {
    }

    bool Intersects(const Rectangle &other)
    {
        // TODO
        return true;
    }
};

#endif