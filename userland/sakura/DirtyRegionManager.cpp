#include "DirtyRegionManager.h"

DirtyRegionManager::DirtyRegionManager()
{
    Reset();
}

void DirtyRegionManager::MarkDirty(const Rectangle &dirty)
{
    if (dirty.x0 < x0)
        x0 = dirty.x0;
    if (dirty.y0 < y0)
        y0 = dirty.y0;
    if (dirty.x1 > x1)
        x1 = dirty.x1;
    if (dirty.y1 > y1)
        y1 = dirty.y1;

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