#include <gfx/font.h>
#include <gfx/util.h>

Font::Font(const nk::String &path)
{
    size_t len;
    auto data = read_file(path.CStr(), &len);
    if (data == nullptr)
        return;

    if (data[0] != 'F' || data[1] != 'N' || data[2] != 'T')
        return;

    file = data;
    glyphs = new Glyph[256];
    name = (const char *)(data + 3);

    auto idx = 3 + strlen(name) + 1;
    width = data[idx];
    idx++;
    height = data[idx];
    idx++;

    uint32_t *glyphBase = (uint32_t *)(data + idx);
    for (size_t i = 0; i < 256; i++)
    {
        Glyph glyph;
        glyph.rows = glyphBase;
        glyph.advance = *((uint8_t*)(glyphBase + height) + 1);
        glyphs[i] = glyph;
        glyphBase += height + 1;
    }
}