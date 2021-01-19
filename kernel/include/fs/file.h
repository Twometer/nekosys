#ifndef _FILE_H
#define _FILE_H
#include <nk/string.h>
#include <stddef.h>

namespace FS
{

    struct File
    {
        nk::String path;
        nk::String name;
        size_t size;
    };

} // namespace FS

#endif