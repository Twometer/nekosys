#ifndef _FILE_H
#define _FILE_H

#include <nk/string.h>
#include <stddef.h>
#include <stdint.h>

namespace FS
{

    enum class DirEntryType
    {
        File,
        Folder,
        Hidden,
        VolumeLabel,
        System,
        Invalid
    };

    struct DirEntry
    {
        static const DirEntry Invalid;

        nk::String name;

        DirEntryType type;

        size_t size;

        uint32_t location;
    };

} // namespace FS

#endif