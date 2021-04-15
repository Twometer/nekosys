#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "point.h"
#include "size.h"
#include "util.h"

struct Rectangle
{
    unsigned int x0;
    unsigned int y0;
    unsigned int x1;
    unsigned int y1;

    Rectangle(Point position, Size size)
    {
        x0 = position.x;
        y0 = position.y;
        x1 = x0 + size.width;
        y1 = y0 + size.height;
    }

    Rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
        : Rectangle({x, y}, {width, height})
    {
    }

    bool Intersects(const Rectangle &rectB) const
    {
        auto &rectA = *this;
        return rectA.x0 < rectB.x1 && rectA.x1 > rectB.x0 && rectA.y0 < rectB.y1 && rectA.y1 > rectB.y0;
    }

    bool Intersects(const Point &point) const
    {
        return point.x >= x0 && point.y > y0 && point.x < x1 && point.y < y1;
    }

    Rectangle Intersection(const Rectangle &other) const
    {
        unsigned int x0 = MAX(this->x0, other.x0);
        unsigned int y0 = MAX(this->y0, other.y0);

        unsigned int x1 = MIN(this->x1, other.x1);
        unsigned int y1 = MIN(this->y1, other.y1);
        return Rectangle(x0, y0, x1 - x0, y1 - y0);
    }

    Point position() const
    {
        return {x0, y0};
    }

    Size size() const
    {
        return {x1 - x0, y1 - y0};
    }
};

#endif