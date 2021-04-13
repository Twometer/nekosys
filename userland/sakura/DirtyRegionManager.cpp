#include "DirtyRegionManager.h"

DirtyRegionManager::DirtyRegionManager()
{
    Reset();
}

void DirtyRegionManager::MarkDirty(const Rectangle &dirty)
{
    if (dirty.position.x < x0)
        x0 = dirty.position.x;
    if (dirty.position.y < y0)
        y0 = dirty.position.y;
    if (dirty.position.x + dirty.size.width > x1)
        x1 = dirty.position.x + dirty.size.width;
    if (dirty.position.y + dirty.size.height > y1)
        y1 = dirty.position.y + dirty.size.height;

    invalidated = true;
}

void DirtyRegionManager::Reset()
{
    invalidated = false;
    x0 = 999999;
    y0 = 999999;
    x1 = 0;
    y1 = 0;
}

Rectangle DirtyRegionManager::GetRectangle()
{
    return {x0, y0, x1 - x0, y1 - y0};
}