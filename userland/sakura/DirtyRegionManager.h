#ifndef _DIRTY_REGION_MANAGER
#define _DIRTY_REGION_MANAGER
#include <gfx/rectangle.h>

class DirtyRegionManager
{
private:
    bool invalidated = false;

    int x0;
    int y0;
    int x1;
    int y1;

public:
    DirtyRegionManager();

    void MarkDirty(const Rectangle &rect);

    void Reset();

    Rectangle GetRectangle();

    bool AnyDirty() const { return invalidated; }
};

#endif