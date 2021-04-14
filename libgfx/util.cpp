#include <gfx/util.h>
#include <stdio.h>

uint8_t *read_file(const char *path, size_t *size)
{
    FILE *fd = fopen(path, "r");
    if (!fd)
        return nullptr;

    fseek(fd, 0, SEEK_END);
    size_t len = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    uint8_t *data = new uint8_t[len];
    fread(data, 1, len, fd);
    fclose(fd);

    *size = len;

    return data;
}