#ifndef _FILE_H
#define _FILE_H
#include <stddef.h>

namespace FS
{

    struct File
    {
        const char *path;
        const char *name;
        size_t size;
    };

} // namespace FS

#endif