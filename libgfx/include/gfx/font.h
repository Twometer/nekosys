#ifndef _FONT_H
#define _FONT_H

#include <stdint.h>
#include <nk/string.h>

struct Glyph
{
    uint32_t *rows;
    unsigned int advance;
};

struct Font
{
    unsigned int width;
    unsigned int height;
    const char *name;
    Glyph *glyphs;
    uint8_t *file;

    Font(const nk::String &path);

    ~Font()
    {
        delete[] glyphs;
        delete[] file;
    }
};

#endif