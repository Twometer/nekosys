#include <gfx/gfx.h>
#include <gfx/pixelformat.h>

void gfx_initialize()
{
    PixelFormat::Rgb24 = new PixFmtRgb24();
    PixelFormat::Rgba32 = new PixFmtRgba32();
    PixelFormat::Bgr32 = new PixFmtBgr32();
}